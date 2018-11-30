//
// Created by shenjun on 18-10-9.
//

#pragma once

#include <memory>
#include "logger/Logger.hpp"
#include "configurations/AppConfiguration.hpp"

extern "C"
{
#include <alsa/asoundlib.h>
}

namespace player
{
    class AlsaPlayer
    {
    public:
        AlsaPlayer(Logger& logger, configuration::wave_pcm_hdr& wavHdr, const std::string& filePath);

        void playAudioFile();

    private:
        int sndPcmOpen(snd_pcm_t** playbackHandle);

        int sndPcmHwParamsMalloc(snd_pcm_hw_params_t** hwParams);

        int sndPcmHwParamsAny(snd_pcm_t* playbackHandle, snd_pcm_hw_params_t* hwParams);

        int sndPcmHwParamsSet(snd_pcm_t* playbackHandle, snd_pcm_hw_params_t* hwParams);

        snd_pcm_format_t formatConvert(int format);

        void printWavHdr(size_t& size);

        void writeDataPCM(FILE* fp, snd_pcm_t* playbackHandle);

    private:
        configuration::wave_pcm_hdr m_wavHdr;
        std::string m_filePath;
        std::unique_ptr<char*> m_dataBuffer{nullptr}; // need care this ptr

        snd_pcm_uframes_t m_frames;
        snd_pcm_uframes_t m_periodSize;
        //snd_pcm_t* m_pPlaybackHandle; // pcm device handler
        //snd_pcm_hw_params_t* m_pHwParams{nullptr}; // hw info and config pcm
        Logger& m_logger;
    };
}
