#pragma once
#include "../gladiapp.hpp"
#include "../gladiapp_ws.hpp"
#include "../gladiapp_ws_request.hpp"
#include "../gladiapp_ws_response.hpp"
#include "../gladiapp_error.hpp"
#include "curl_http_util.hpp"

#include <curl/curl.h>
#include <sstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif

using namespace gladiapp::v2::response;
using namespace gladiapp::v2::ws::response;
using namespace gladiapp::v2::ws::request;

namespace gladiapp::v2::ws
{
    class GladiaWebsocketClientImpl
    {
    public:
        GladiaWebsocketClientImpl(const std::string &apiKey, const std::string &caFilePath = {})
            : _apiKey(apiKey), _caFilePath(caFilePath)
        {
        }

        ~GladiaWebsocketClientImpl()
        {
        }

        InitializeSessionResponse connect(const InitializeSessionRequest &initRequest,
                                          TranscriptionError *transcriptionError) const
        {
            try
            {
                std::string region;
                switch (initRequest.region)
                {
                case InitializeSessionRequest::Region::US_WEST:
                    region = "us-west";
                    break;
                case InitializeSessionRequest::Region::EU_WEST:
                    region = "eu-west";
                    break;
                }

                std::ostringstream oss;
                oss << gladiapp::v2::common::LIVE_ENDPOINT << "?region=" << region;

                auto httpResponse = gladiapp::v2::curl_util::performRequest(
                    gladiapp::v2::curl_util::buildUrl(oss.str()), "POST", _apiKey,
                    initRequest.toJson().dump(), "application/json", _caFilePath);

                if (httpResponse.statusCode == 201)
                {
                    InitializeSessionResponse initResponse = InitializeSessionResponse::fromJson(nlohmann::json::parse(httpResponse.body));
                    spdlog::info("Session initialized successfully: {}, {}", initResponse.id, initResponse.url);
                    return initResponse;
                }
                else
                {
                    spdlog::error("Error: {}", httpResponse.body);
                    if (transcriptionError != nullptr)
                    {
                        *transcriptionError = TranscriptionError::fromJson(httpResponse.body);
                    }
                }
            }
            catch (std::exception &e)
            {
                spdlog::error("Error occurred: {}", e.what());
            }
            return InitializeSessionResponse{};
        }

        bool getResultById(const std::string &id,
                           nlohmann::json &outputJson,
                           gladiapp::v2::response::TranscriptionError *transcriptionError) const
        {
            try
            {
                auto httpResponse = gladiapp::v2::curl_util::performRequest(
                    gladiapp::v2::curl_util::buildUrl(std::string(gladiapp::v2::common::LIVE_ENDPOINT) + "/" + id),
                    "GET", _apiKey, "", "application/json", _caFilePath);

                if (httpResponse.statusCode != 200)
                {
                    spdlog::error("Error getting live transcription session result: {}", httpResponse.body);
                    if (transcriptionError != nullptr)
                    {
                        *transcriptionError = TranscriptionError::fromJson(httpResponse.body);
                    }
                    return false;
                }
                outputJson = nlohmann::json::parse(httpResponse.body);
            }
            catch (std::exception &e)
            {
                spdlog::error("Error occurred: {}", e.what());
                return false;
            }
            return true;
        }

        bool deleteResultById(const std::string &id,
                              gladiapp::v2::response::TranscriptionError *transcriptionError) const
        {
            try
            {
                auto httpResponse = gladiapp::v2::curl_util::performRequest(
                    gladiapp::v2::curl_util::buildUrl(std::string(gladiapp::v2::common::LIVE_ENDPOINT) + "/" + id),
                    "DELETE", _apiKey, "", "application/json", _caFilePath);

                if (httpResponse.statusCode != 202)
                {
                    spdlog::error("Error deleting session: {}", httpResponse.body);
                    if (transcriptionError != nullptr)
                    {
                        *transcriptionError = TranscriptionError::fromJson(httpResponse.body);
                    }
                    return false;
                }
            }
            catch (std::exception &e)
            {
                spdlog::error("Error occurred: {}", e.what());
                return false;
            }
            return true;
        }

    private:
        std::string _apiKey;
        std::string _caFilePath;
    };

    class GladiaWebsocketClientSessionImpl
    {
    public:
        GladiaWebsocketClientSessionImpl(const std::string &endpoint, const std::string &caFilePath = {})
            : _endpoint(endpoint),
              _caFilePath(caFilePath),
              _curl(nullptr),
              _keepReading(false),
              _canSendData(true)
        {
            gladiapp::v2::curl_util::ensureGlobalInit();
        }

        ~GladiaWebsocketClientSessionImpl()
        {
            _keepReading = false;
            if (_dataReceptionThread.joinable())
            {
                _dataReceptionThread.join();
            }
            disconnect();
        }

        bool connectAndStart(const std::function<void(const std::string &)> &dataReadCallback,
                             const std::function<void()> &onConnectedCallback = nullptr,
                             const std::function<void(const std::string &message)> &onDisconnectedCallback = nullptr,
                             const std::function<void(const std::string &errorMessage)> &onErrorCallback = nullptr)
        {
            if (!connect())
            {
                return false;
            }
            if (!startThread(dataReadCallback, onConnectedCallback, onDisconnectedCallback, onErrorCallback))
            {
                disconnect();
                return false;
            }
            return true;
        }

        bool sendStopSignal()
        {
            if (_canSendData)
            {
                _canSendData = false;
                nlohmann::json stopJson = {{"type", "stop_recording"}};
                std::string payload = stopJson.dump();
                if (!sendFrame(payload.data(), payload.size(), CURLWS_TEXT))
                {
                    spdlog::error("Error sending stop signal");
                    return false;
                }
                spdlog::info("Sent stop signal to WebSocket.");
            }
            return true;
        }

        void disconnect()
        {
            std::lock_guard<std::mutex> lock(_sendMutex);
            if (_curl != nullptr)
            {
                size_t sent = 0;
                curl_ws_send(_curl, "", 0, &sent, 0, CURLWS_CLOSE);
                curl_easy_cleanup(_curl);
                _curl = nullptr;
                spdlog::info("WebSocket disconnected.");
            }
        }

        bool isConnected() const
        {
            return _curl != nullptr;
        }

        bool sendAudioBinary(const uint8_t *audioData, int size,
                             std::function<void(const std::string &)> errorCallback = nullptr) const
        {
            if (!isConnected())
            {
                spdlog::warn("WebSocket is not open. Cannot send audio binary data.");
                return false;
            }
            if (!_canSendData)
            {
                spdlog::warn("Cannot send audio data after stop signal has been sent.");
                return false;
            }
            if (!sendFrame(reinterpret_cast<const char *>(audioData), static_cast<size_t>(size), CURLWS_BINARY))
            {
                if (errorCallback)
                {
                    errorCallback("Error sending audio binary data");
                }
                return false;
            }
            spdlog::debug("Sent {} bytes of audio binary data.", size);
            return true;
        }

        bool sendTextJson(const std::string &jsonText,
                          std::function<void(const std::string &)> errorCallback = nullptr) const
        {
            if (!isConnected())
            {
                spdlog::warn("WebSocket is not open. Cannot send text data.");
                return false;
            }
            if (!_canSendData)
            {
                spdlog::warn("Cannot send text data after stop signal has been sent.");
                return false;
            }
            if (!sendFrame(jsonText.data(), jsonText.size(), CURLWS_TEXT))
            {
                if (errorCallback)
                {
                    errorCallback("Error sending JSON text data");
                }
                return false;
            }
            spdlog::debug("Sent JSON text data: {}", jsonText);
            return true;
        }

    private:
        bool sendFrame(const char *data, size_t len, unsigned int flags) const
        {
            std::lock_guard<std::mutex> lock(_sendMutex);
            if (_curl == nullptr)
            {
                return false;
            }
            size_t sent = 0;
            CURLcode res = curl_ws_send(_curl, data, len, &sent, 0, flags);
            if (res != CURLE_OK)
            {
                spdlog::error("curl_ws_send failed: {}", curl_easy_strerror(res));
                return false;
            }
            return true;
        }

        static bool waitSocketReadable(curl_socket_t sockfd, long timeoutMs)
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            struct timeval tv;
            tv.tv_sec = timeoutMs / 1000;
            tv.tv_usec = (timeoutMs % 1000) * 1000;
            int rc = select(static_cast<int>(sockfd) + 1, &readfds, nullptr, nullptr, &tv);
            return rc > 0;
        }

        bool connect()
        {
            spdlog::info("Connecting to {} ...", _endpoint);
            CURL *curl = curl_easy_init();
            if (!curl)
            {
                spdlog::error("Failed to initialize curl easy handle");
                return false;
            }

            curl_easy_setopt(curl, CURLOPT_URL, _endpoint.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, gladiapp::v2::common::USER_AGENT);
            gladiapp::v2::curl_util::applyCaFile(curl, _caFilePath);
            // 2L: connect and prepare the handle for use with curl_ws_send/curl_ws_recv
            curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                spdlog::error("Error occurred: {}", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                return false;
            }

            _curl = curl;
            spdlog::info("WebSocket connected successfully!");
            return true;
        }

        bool startThread(const std::function<void(const std::string &)> &dataReadCallback,
                         const std::function<void()> &onConnectedCallback = nullptr,
                         const std::function<void(const std::string &message)> &onDisconnectedCallback = nullptr,
                         const std::function<void(const std::string &errorMessage)> &onErrorCallback = nullptr)
        {
            _dataReceptionThread = std::thread([this, dataReadCallback, onConnectedCallback, onDisconnectedCallback, onErrorCallback]()
                                               {
                _keepReading = true;
                curl_socket_t sockfd = CURL_SOCKET_BAD;
                curl_easy_getinfo(_curl, CURLINFO_ACTIVESOCKET, &sockfd);

                bool isConnectedFlag = false;
                std::string messageAccumulator;
                std::vector<char> buffer(65536);

                while (_curl != nullptr && _keepReading)
                {
                    size_t bytesRead = 0;
                    const curl_ws_frame *meta = nullptr;
                    CURLcode res = curl_ws_recv(_curl, buffer.data(), buffer.size(), &bytesRead, &meta);

                    if (res == CURLE_AGAIN)
                    {
                        waitSocketReadable(sockfd, 200);
                        continue;
                    }

                    if (res != CURLE_OK)
                    {
                        std::string errorMessage = curl_easy_strerror(res);
                        spdlog::warn("WebSocket closed by server: {}", errorMessage);
                        if (onDisconnectedCallback)
                        {
                            onDisconnectedCallback(errorMessage);
                        }
                        break;
                    }

                    if (!isConnectedFlag)
                    {
                        isConnectedFlag = true;
                        if (onConnectedCallback)
                        {
                            onConnectedCallback();
                        }
                    }

                    if (meta != nullptr && (meta->flags & CURLWS_CLOSE) != 0)
                    {
                        spdlog::info("WebSocket close frame received.");
                        if (onDisconnectedCallback)
                        {
                            onDisconnectedCallback("closed by server");
                        }
                        break;
                    }

                    messageAccumulator.append(buffer.data(), bytesRead);
                    if (meta == nullptr || meta->bytesleft == 0)
                    {
                        dataReadCallback(messageAccumulator);
                        messageAccumulator.clear();
                    }
                } });
            return true;
        }

    private:
        std::string _endpoint;
        std::string _caFilePath;
        CURL *_curl;
        mutable std::mutex _sendMutex;
        std::thread _dataReceptionThread;
        std::atomic<bool> _keepReading;
        std::atomic<bool> _canSendData;
    };
}
