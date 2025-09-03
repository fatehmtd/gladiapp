#pragma once
#include "../gladiapp.hpp"
#include "../gladiapp_ws.hpp"
#include "../gladiapp_ws_request.hpp"
#include "../gladiapp_ws_response.hpp"
#include "../gladiapp_error.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <sstream>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

using Websocket = websocket::stream<ssl::stream<tcp::socket>>;

using namespace gladiapp::v2::response;
using namespace gladiapp::v2::ws::response;
using namespace gladiapp::v2::ws::request;

namespace gladiapp::v2::ws
{
    class GladiaWebsocketClientImpl
    {
    public:
        GladiaWebsocketClientImpl(const std::string &apiKey) : _apiKey(apiKey), _resolver(_ioContext)
        {
        }

        ~GladiaWebsocketClientImpl()
        {
        }

        InitializeSessionResponse connect(const InitializeSessionRequest &initRequest,
                                          TranscriptionError *transcriptionError)
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

                spdlog::info("Response: {}", responseBody);

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

    private:
        std::string _apiKey;
        mutable net::io_context _ioContext;
        mutable tcp::resolver _resolver;
    };

    class GladiaWebsocketClientSessionImpl
    {
    public:
        GladiaWebsocketClientSessionImpl(const std::string &endpoint) : _endpoint(endpoint), _sslContext(boost::asio::ssl::context::sslv23_client)
        {
        }

        bool connectAndStart()
        {
            return false;
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

        void processData(const std::string &data)
        {
        }

    private:
        std::string _endpoint;
        boost::asio::io_context _ioContext;
        boost::beast::net::ssl::context _sslContext;
        Websocket _webSocket;
        std::thread _dataReceptionThread;
    };
}