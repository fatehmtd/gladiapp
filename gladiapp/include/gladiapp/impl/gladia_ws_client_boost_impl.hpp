#pragma once
#include "../gladiapp.hpp"
#include "../gladiapp_ws.hpp"
#include "../gladiapp_ws_request.hpp"
#include "../gladiapp_ws_response.hpp"
#include "../gladiapp_error.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/beast/http.hpp>
#include <sstream>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <functional>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

using Websocket = boost::beast::websocket::stream<ssl::stream<tcp::socket>>;

using namespace gladiapp::v2::response;
using namespace gladiapp::v2::ws::response;
using namespace gladiapp::v2::ws::request;

namespace gladiapp::v2::ws
{
    class GladiaWebsocketClientImpl
    {
    public:
        GladiaWebsocketClientImpl(const std::string &apiKey) : _apiKey(apiKey),
                                                               _resolver(_ioContext)
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
                ssl::context sslContext(ssl::context::sslv23_client);
                auto const results = _resolver.resolve(gladiapp::v2::common::HOST, "443");

                beast::tcp_stream tcpStream(_ioContext);
                tcpStream.connect(results);

                ssl::stream<beast::tcp_stream> sslStream(std::move(tcpStream), sslContext);
                sslStream.handshake(ssl::stream_base::client);

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
                oss << "?region=" << region;

                // Use buffer_body with the multipart data
                http::request<http::string_body> httpRequest{
                    http::verb::post,
                    gladiapp::v2::common::LIVE_ENDPOINT + oss.str(),
                    11};
                httpRequest.set(http::field::host, gladiapp::v2::common::HOST);
                httpRequest.set(gladiapp::v2::headers::X_GLADIA_KEY, _apiKey);
                httpRequest.set(http::field::user_agent, gladiapp::v2::common::USER_AGENT);
                httpRequest.set(http::field::content_type, "application/json");
                httpRequest.body() = initRequest.toJson().dump();
                httpRequest.prepare_payload();

                // Send using Beast's serialization
                http::write(sslStream, httpRequest);

                // Read response
                beast::flat_buffer responseBuffer;
                http::response<http::string_body> httpResponse;
                http::read(sslStream, responseBuffer, httpResponse);

                std::string responseBody = httpResponse.body();

                if (httpResponse.result() == http::status::created)
                {
                    InitializeSessionResponse initResponse = InitializeSessionResponse::fromJson(nlohmann::json::parse(responseBody));
                    spdlog::info("Session initialized successfully: {}, {}", initResponse.id, initResponse.url);
                    return initResponse;
                }
                else
                {
                    spdlog::error("Error: {}", responseBody);
                    if (transcriptionError != nullptr)
                    {
                        *transcriptionError = TranscriptionError::fromJson(responseBody);
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
                ssl::context sslContext(ssl::context::sslv23_client);
                auto const results = _resolver.resolve(gladiapp::v2::common::HOST, "443");

                beast::tcp_stream tcpStream(_ioContext);
                tcpStream.connect(results);

                ssl::stream<beast::tcp_stream> sslStream(std::move(tcpStream), sslContext);
                sslStream.handshake(ssl::stream_base::client);

                // Use buffer_body with the multipart data
                http::request<http::empty_body> httpRequest{
                    http::verb::get,
                    std::string(gladiapp::v2::common::LIVE_ENDPOINT) + "/" + id,
                    11};
                httpRequest.set(http::field::host, gladiapp::v2::common::HOST);
                httpRequest.set(gladiapp::v2::headers::X_GLADIA_KEY, _apiKey);
                httpRequest.set(http::field::user_agent, gladiapp::v2::common::USER_AGENT);
                httpRequest.set(http::field::content_type, "application/json");
                httpRequest.prepare_payload();

                // Send using Beast's serialization
                http::write(sslStream, httpRequest);

                // Read response
                beast::flat_buffer responseBuffer;
                http::response<http::string_body> httpResponse;
                http::read(sslStream, responseBuffer, httpResponse);
                if(httpResponse.result() != http::status::ok) {
                    spdlog::error("Error getting session result: {}", httpResponse.body());
                    if (transcriptionError != nullptr)
                    {
                        *transcriptionError = TranscriptionError::fromJson(httpResponse.body());
                    }
                    return false;
                }
                outputJson = nlohmann::json::parse(httpResponse.body());
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
                ssl::context sslContext(ssl::context::sslv23_client);
                auto const results = _resolver.resolve(gladiapp::v2::common::HOST, "443");

                beast::tcp_stream tcpStream(_ioContext);
                tcpStream.connect(results);

                ssl::stream<beast::tcp_stream> sslStream(std::move(tcpStream), sslContext);
                sslStream.handshake(ssl::stream_base::client);

                // Use buffer_body with the multipart data
                http::request<http::empty_body> httpRequest{
                    http::verb::delete_,
                    std::string(gladiapp::v2::common::LIVE_ENDPOINT) + "/" + id,
                    11};
                httpRequest.set(http::field::host, gladiapp::v2::common::HOST);
                httpRequest.set(gladiapp::v2::headers::X_GLADIA_KEY, _apiKey);
                httpRequest.set(http::field::user_agent, gladiapp::v2::common::USER_AGENT);
                httpRequest.set(http::field::content_type, "application/json");
                httpRequest.prepare_payload();

                // Send using Beast's serialization
                http::write(sslStream, httpRequest);

                // Read response
                beast::flat_buffer responseBuffer;
                http::response<http::string_body> httpResponse;
                http::read(sslStream, responseBuffer, httpResponse);
                if(httpResponse.result() != http::status::accepted) {
                    spdlog::error("Error deleting session: {}", httpResponse.body());
                    if (transcriptionError != nullptr)
                    {
                        *transcriptionError = TranscriptionError::fromJson(httpResponse.body());
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
        mutable net::io_context _ioContext;
        mutable tcp::resolver _resolver;
    };

    class GladiaWebsocketClientSessionImpl
    {
    public:
        GladiaWebsocketClientSessionImpl(const std::string &endpoint) : _endpoint(endpoint),
                                                                        _sslContext(boost::asio::ssl::context::sslv23_client),
                                                                        _webSocket(_ioContext, _sslContext),
                                                                        _canSendData(true)
        {
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
                try
                {
                    _webSocket.text(true);
                    nlohmann::json stopJson = {{"type", "stop_recording"}};
                    _webSocket.write(net::buffer(stopJson.dump()));
                    spdlog::info("Sent stop signal to WebSocket.");
                }
                catch (std::exception &e)
                {
                    spdlog::error("Error sending stop signal: {}", e.what());
                    return false;
                }
            }
            return true;
        }

        void disconnect()
        {
            if (_webSocket.is_open())
            {
                try
                {
                    _webSocket.close(beast::websocket::close_code::normal);
                    spdlog::info("WebSocket disconnected.");
                }
                catch (std::exception &e)
                {
                    spdlog::error("Error during WebSocket close: {}", e.what());
                }
            }
        }

        bool isConnected() const
        {
            return _webSocket.is_open();
        }

        bool sendAudioBinary(const uint8_t *audioData, int size,
                             std::function<void(const std::string &)> errorCallback = nullptr) const
        {
            if (!_webSocket.is_open())
            {
                spdlog::warn("WebSocket is not open. Cannot send audio binary data.");
                return false;
            }
            if (!_canSendData)
            {
                spdlog::warn("Cannot send audio data after stop signal has been sent.");
                return false;
            }
            try
            {
                _webSocket.binary(true);
                beast::error_code ec;
                _webSocket.write(net::buffer(audioData, size), ec);
                if (ec)
                {
                    spdlog::error("Error sending audio binary data: {}", ec.message());
                    if (errorCallback)
                    {
                        errorCallback(ec.message());
                    }
                    return false;
                }
                spdlog::debug("Sent {} bytes of audio binary data.", size);
            }
            catch (std::exception &e)
            {
                spdlog::error("Error sending audio binary data: {}", e.what());
                return false;
            }
            return true;
        }

        bool sendTextJson(const std::string &jsonText,
                          std::function<void(const std::string &)> errorCallback = nullptr) const
        {
            if (!_webSocket.is_open())
            {
                spdlog::warn("WebSocket is not open. Cannot send text data.");
                return false;
            }
            if (!_canSendData)
            {
                spdlog::warn("Cannot send text data after stop signal has been sent.");
                return false;
            }
            try
            {
                _webSocket.text(true);
                beast::error_code ec;
                _webSocket.write(net::buffer(jsonText), ec);
                if (ec)
                {
                    spdlog::error("Error sending JSON text data: {}", ec.message());
                    if (errorCallback)
                    {
                        errorCallback(ec.message());
                    }
                    return false;
                }
                spdlog::debug("Sent JSON text data: {}", jsonText);
            }
            catch (std::exception &e)
            {
                spdlog::error("Error sending JSON text data: {}", e.what());
                return false;
            }
            return true;
        }

    private:
        std::pair<std::string, std::string> separateHostFromTarget(const std::string &url)
        {
            // Find the position after the protocol (wss:// or ws://)
            size_t protocolEnd = url.find("://");
            if (protocolEnd == std::string::npos)
            {
                return {url, "/"};
            }

            // Start after the protocol
            size_t hostStart = protocolEnd + 3;

            // Find the first '/' after the protocol
            size_t pathStart = url.find('/', hostStart);
            if (pathStart == std::string::npos)
            {
                // No path found, return host without protocol and default target
                return {url.substr(hostStart), "/"};
            }

            std::string host = url.substr(hostStart, pathStart - hostStart);
            std::string target = url.substr(pathStart);

            return {host, target};
        }

        bool connect()
        {
            try
            {
                std::pair<std::string, std::string> hostAndTarget = separateHostFromTarget(_endpoint);
                tcp::resolver resolver{_ioContext};
                spdlog::info("Connecting to {}:{} ...", hostAndTarget.first, hostAndTarget.second);
                auto const results = resolver.resolve(hostAndTarget.first, "443");
                net::connect(_webSocket.next_layer().lowest_layer(), results.begin(), results.end());

                spdlog::info("Performing SSL handshake...");
                _webSocket.next_layer().set_verify_callback(ssl::host_name_verification(hostAndTarget.first));
                _webSocket.next_layer().handshake(ssl::stream_base::client);

                _webSocket.handshake(hostAndTarget.first, hostAndTarget.second);
                spdlog::info("WebSocket connected successfully!");
            }
            catch (std::exception &e)
            {
                spdlog::error("Error occurred: {}", e.what());
                return false;
            }
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
                try
                {
                    beast::flat_buffer buffer;
                    beast::error_code ec;
                    bool isConnected = false;
                    while (_webSocket.is_open() && _keepReading)
                    {                        
                        size_t bytesRead = _webSocket.read(buffer, ec);
                        if (ec)
                        {
                            // Check if it's a normal WebSocket closure
                            if (ec == beast::websocket::error::closed || 
                                ec == net::error::eof ||
                                ec == net::ssl::error::stream_truncated)
                            {
                                spdlog::warn("WebSocket closed by server: {}", ec.message());
                                if(onDisconnectedCallback)
                                {
                                    onDisconnectedCallback(ec.message());
                                }
                            }
                            else
                            {
                                spdlog::error("Error reading from WebSocket: {}, {}, {}", ec.message(), ec.value(), ec.category().name());
                                if (onErrorCallback)
                                {
                                    onErrorCallback(ec.message());
                                }
                                if(onDisconnectedCallback)
                                {
                                    onDisconnectedCallback(ec.message());
                                }
                            }
                            break;
                        } else {
                            if(!isConnected) {
                                isConnected = true;
                                if(onConnectedCallback) {
                                    onConnectedCallback();
                                }
                            }
                        }
                        std::string data(beast::buffers_to_string(buffer.data()));
                        buffer.consume(bytesRead);
                        dataReadCallback(data);
                    }
                }
                catch (std::exception &e)
                {
                    spdlog::error("Error in data reception thread: {}", e.what());
                } });
            return true;
        }

    private:
        std::string _endpoint;
        net::io_context _ioContext;
        boost::beast::net::ssl::context _sslContext;
        mutable Websocket _webSocket;
        std::thread _dataReceptionThread;
        bool _keepReading;
        bool _canSendData;
    };
}