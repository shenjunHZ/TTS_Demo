//
// Created by shenjun on 18-10-9.
//

#include <thread>
#include <errno.h>
#include "AlsaPlayer.hpp"

namespace
{
    std::atomic_bool keep_running(true);
}

namespace player
{
    AlsaPlayer::AlsaPlayer(Logger &logger, configuration::wave_pcm_hdr& wavHdr, const std::string& filePath)
        : m_wavHdr{wavHdr}
        , m_filePath{filePath}
        , m_logger{logger}
    {

    }

    void AlsaPlayer::printWavHdr(size_t& size)
    {
        LOG_DEBUG_MSG("read size: {}", size);
        LOG_DEBUG_MSG("RiffID: {}{}{}{}", m_wavHdr.riff[0], m_wavHdr.riff[1], m_wavHdr.riff[2], m_wavHdr.riff[3]);
        LOG_DEBUG_MSG("RiffSize: {}", m_wavHdr.size_8);
        LOG_DEBUG_MSG("WaveID: {}{}{}{}", m_wavHdr.wave[0], m_wavHdr.wave[1], m_wavHdr.wave[2], m_wavHdr.wave[3]);
        LOG_DEBUG_MSG("FmtID: {}{}{}{}", m_wavHdr.fmt[0], m_wavHdr.fmt[1], m_wavHdr.fmt[2], m_wavHdr.fmt[3]);
        LOG_DEBUG_MSG("FmtSize: {}", m_wavHdr.fmt_size);
        LOG_DEBUG_MSG("wFormatTag: {}", m_wavHdr.format_tag);
        LOG_DEBUG_MSG("nChannels: {}", m_wavHdr.channels);
        LOG_DEBUG_MSG("nSamplesPerSec: {}", m_wavHdr.samples_per_sec);
        LOG_DEBUG_MSG("nAvgBytesPerSec: {}", m_wavHdr.avg_bytes_per_sec);
        LOG_DEBUG_MSG("nBlockAlign: {}", m_wavHdr.block_align);
        LOG_DEBUG_MSG("wBitsPerSample: {}", m_wavHdr.bits_per_sample);
        LOG_DEBUG_MSG("DataID: {}{}{}{}", m_wavHdr.data[0], m_wavHdr.data[1], m_wavHdr.data[2], m_wavHdr.data[3]);
        LOG_DEBUG_MSG("nDataBytes: {}", m_wavHdr.data_size);
    }

    int AlsaPlayer::sndPcmOpen(snd_pcm_t** playbackHandle)
    {
        // 打开PCM，最后一个参数为0意味着标准配置
        int ret = snd_pcm_open(playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm open error {}", snd_strerror(ret));
        }
        return ret;
    }

    int AlsaPlayer::sndPcmHwParamsMalloc(snd_pcm_hw_params_t** hwParams)
    {
        // 分配snd_pcm_hw_params_t结构体
        int ret = snd_pcm_hw_params_malloc(hwParams);
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm hw params malloc error {}", snd_strerror(ret));
        }
        return ret;
    }

    int AlsaPlayer::sndPcmHwParamsAny(snd_pcm_t* playbackHandle, snd_pcm_hw_params_t* hwParams)
    {
        // 初始化hw_params
        int ret = snd_pcm_hw_params_any(playbackHandle, hwParams);
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm hw params any error {}", snd_strerror(ret));
        }
        return ret;
    }

    snd_pcm_format_t AlsaPlayer::formatConvert(int format)
    {
        switch(format)
        {
            case 8:
                return SND_PCM_FORMAT_U8;
            case 16:
                return SND_PCM_FORMAT_S16_LE;
            case 24:
                return SND_PCM_FORMAT_U24_LE;
            case 32:
                return SND_PCM_FORMAT_U32_LE;
            default:
                return SND_PCM_FORMAT_S16_LE;
        }
    }

    int AlsaPlayer::sndPcmHwParamsSet(snd_pcm_t* playbackHandle, snd_pcm_hw_params_t* hwParams)
    {
        // 初始化访问权限
        int ret = snd_pcm_hw_params_set_access(playbackHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm hw params set access error {}", snd_strerror(ret));
            return ret;
        }

        // 初始化采样格式SND_PCM_FORMAT_U8,8位
        ret = snd_pcm_hw_params_set_format(playbackHandle, hwParams, formatConvert(m_wavHdr.fmt_size));
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm hw params set format error {}", snd_strerror(ret));
            return ret;
        }

        // 设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的
        int dir = 0;
        uint32_t rate = m_wavHdr.samples_per_sec;
        ret = snd_pcm_hw_params_set_rate_near(playbackHandle, hwParams, &rate, &dir);
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm hw params set rate error {}", snd_strerror(ret));
            return ret;
        }

        // 设置通道数量  1
        ret = snd_pcm_hw_params_set_channels(playbackHandle, hwParams, m_wavHdr.channels);
        if(0 > ret)
        {
            LOG_ERROR_MSG("pcm hw params set channels error {}", snd_strerror(ret));
            return ret;
        }

        /* Set period size to 32 frames. */
        m_frames = 32;
        ret = snd_pcm_hw_params_set_buffer_size_near(playbackHandle, hwParams, &m_frames);
        if (0 > ret)
        {
            LOG_ERROR_MSG("Unable to set buffer size {} : {}", m_frames * 2, snd_strerror(ret));
            return ret;
        }
        m_periodSize = m_frames / 2;

        ret = snd_pcm_hw_params_set_period_size_near(playbackHandle, hwParams, &m_periodSize, nullptr);
        if (0 > ret)
        {
            LOG_ERROR_MSG("Unable to set period size {} : {}", m_frames,  snd_strerror(ret));
            return ret;
        }

        // 设置hw_params
        ret = snd_pcm_hw_params(playbackHandle, hwParams);
        if (0 > ret)
        {
            LOG_ERROR_MSG("snd_pcm_hw_params {}", snd_strerror(ret));
            return ret;
        }

        /* Use a buffer large enough to hold one period */
        ret = snd_pcm_hw_params_get_period_size(hwParams, &m_frames, &dir);
        if (0 > ret)
        {
            LOG_ERROR_MSG("snd_pcm_hw_params_get_period {}", snd_strerror(ret));
            return ret;
        }

        LOG_DEBUG_MSG("frames size: {} dir: {}", m_frames, dir);
        if(m_dataBuffer)
        {
            m_dataBuffer.release();
        }
        m_dataBuffer = std::make_unique<char*>(new char[m_frames * 2]);

        return ret;
    }

    void AlsaPlayer::playAudioFile()
    {
        snd_pcm_t* playbackHandle = nullptr; // pcm device handler
        snd_pcm_hw_params_t* hwParams = nullptr; // hw info and config pcm
        FILE* fp = nullptr;

        if(0 > sndPcmOpen(&playbackHandle))
        {
            return;
        }

        if(0 > sndPcmHwParamsMalloc(&hwParams))
        {
            snd_pcm_close(playbackHandle);
            return;
        }

        if(0 > sndPcmHwParamsAny(playbackHandle, hwParams))
        {
            snd_pcm_close(playbackHandle);
            return;
        }

        if(0 > sndPcmHwParamsSet(playbackHandle, hwParams))
        {
            snd_pcm_close(playbackHandle);
            return;
        }

        fp = fopen(m_filePath.c_str(), "rb");
        if (nullptr == fp)
        {
            LOG_ERROR_MSG("open {} error.", m_filePath.c_str());
            return;
        }
        memset(&m_wavHdr, 0, sizeof(m_wavHdr));
        size_t size = fread(&m_wavHdr, 1, sizeof(m_wavHdr), fp);
        printWavHdr(size);

        //std::thread threadAudio(&AlsaPlayer::writeDataPCM, this, fp, playbackHandle);
        //if(threadAudio.joinable())
        //{
        //    threadAudio.join();
        //}
        writeDataPCM(fp, playbackHandle);
        fclose(fp);
    }

    void AlsaPlayer::writeDataPCM(FILE* fp, snd_pcm_t* playbackHandle)
    {
        while(keep_running)
        {
            int ret = fread(m_dataBuffer.get(), 1, m_frames * 2, fp);
            if(1 > ret)
            {
                LOG_DEBUG_MSG("end of file on input.");
                break;
            }

            // 写音频数据到PCM设备
            while((ret = snd_pcm_writei(playbackHandle, m_dataBuffer.get(), m_frames)) < 0)
            {
                usleep(2000);
                if (ret == -EPIPE)
                {
                    // Broken pipe
                    LOG_ERROR_MSG("under run occurred.");
                    // 完成硬件参数设置，使设备准备好
                    snd_pcm_prepare(playbackHandle);
                }
                else if (ret < 0)
                {
                    LOG_ERROR_MSG("error from writei: {}", snd_strerror(ret));
                }
            }
        }
        // 关闭PCM设备句柄
        LOG_DEBUG_MSG("111");
        snd_pcm_drain(playbackHandle);
        LOG_DEBUG_MSG("222");
        snd_pcm_close(playbackHandle);
        LOG_DEBUG_MSG("333");
    }

} // namespace player