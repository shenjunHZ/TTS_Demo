//
// Created by shenjun on 18-9-26.
//
#include <memory>
#include "applications/ZmqReceiver.hpp"
#include "logger/Logger.hpp"
#include "boost/exception/diagnostic_information.hpp"

namespace applications
{
    void ZmqReceiver::registerReceiver(zmq::socket_t&& socket, std::function<void(const zmq::message_t&)> messageHandler)
    {
        zmqSockets.push_back(std::move(socket));
        auto& addedSocket = zmqSockets.back();

        std::function<void()> callback =
                [&addedSocket, handler = std::move(messageHandler), &received = this->received, &wrapper = this->wrapper]() {
                    wrapper.recv(addedSocket, received);
                    handler(received);
                };
    }

    void ZmqReceiver::registerReceiver(zmq::socket_t& socket, std::function<void()> callback)
    {
        zmq_pollitem_t pollItem{socket, 0, ZMQ_POLLIN, 0};
        insertPollItemAndCallback(pollItem, std::move(callback));
    }

    void ZmqReceiver::shutdown()
    {
        m_bStop = true;
    }

    void ZmqReceiver::registerReceiverForFileDescriptor(
            const int descriptor,
            std::function<void()> callback)
    {
        zmq_pollitem_t pollItem{nullptr, descriptor, ZMQ_POLLIN, 0};
        insertPollItemAndCallback(pollItem, std::move(callback));
    }

    void ZmqReceiver::unregisterReceiverForFileDescriptor(int descriptor)
    {
        for (auto i = 0u; i < updatedPollItems.size(); ++i)
        {
            if (updatedPollItems.at(i).fd == descriptor)
            {
                updatedPollItems.erase(updatedPollItems.begin() + i);
                updatedCallbacks.erase(updatedCallbacks.begin() + i);
                return;
            }
        }
        BOOST_THROW_EXCEPTION(std::out_of_range("Trying to deregister unknown file descriptor"));
    }

    void ZmqReceiver::receiveLoop() try
    {
        //Expects(pollItems.size() == callbacks.size());

        while (not m_bStop)
        {
            pollItems = updatedPollItems;
            callbacks = updatedCallbacks;
            wrapper.poll(pollItems);
            for (auto i = 0u; i < pollItems.size(); ++i)
            {
                if ((pollItems.at(i).revents & ZMQ_POLLIN) != 0)
                {
                    callbacks.at(i)();
                }
            }
        }
        LOG_DEBUG_MSG("ZmqReceiver received stop message, exiting from receiveLoop");
    }
    catch (...)
    {
        LOG_ERROR_MSG("Unhandled exception in ZmqReceiver loop: {}", boost::current_exception_diagnostic_information());
        throw;
    }

    void ZmqReceiver::unregisterReceiver(const zmq::socket_t& socket)
    {
        for (std::size_t i = 0; i < updatedPollItems.size(); i++)
        {
            if (updatedPollItems[i].socket == socket)
            {
                updatedPollItems.erase(updatedPollItems.begin() + i);
                updatedCallbacks.erase(updatedCallbacks.begin() + i);
                return;
            }
        }
        BOOST_THROW_EXCEPTION(std::out_of_range("Trying to deregister unknown socket"));
    }

    void ZmqReceiver::insertPollItemAndCallback(const zmq_pollitem_t pollItem, std::function<void()> callback)
    {
        updatedPollItems.push_back(pollItem);
        updatedCallbacks.push_back(std::move(callback));
    }
}


