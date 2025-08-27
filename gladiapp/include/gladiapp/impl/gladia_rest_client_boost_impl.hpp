#pragma once

#include "../gladiapp_rest.hpp"
#include "../gladiapp.hpp"
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
#include "../utils.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

using namespace gladiapp::v2;

namespace gladiapp
{
    namespace v2
    {

        class GladiaRestClientImpl
        {
        public:
            GladiaRestClientImpl(const std::string &apiKey) : _apiKey(apiKey), _resolver(_ioContext)
            {
            }

            ~GladiaRestClientImpl()
            {
            }

            response::UploadResponse upload(const std::string &filePath)
            {
                try
                {
                    // Open file
                    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
                    if (!file.is_open())
                    {
                        throw std::runtime_error("Cannot open file: " + filePath);
                    }

                    std::size_t fileSize = file.tellg();
                    file.seekg(0, std::ios::beg);

                    spdlog::info("file name: {} file size: {}", std::filesystem::path(filePath).filename().string(), gladiapp::utils::formatBytes(fileSize));

                    // Create a vector to hold the complete multipart data
                    std::vector<char> multipartData;

                    // Generate boundary and build multipart content
                    std::string boundary = "----formdata-gladiapp-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());

                    // Build the header
                    std::ostringstream headerStream;
                    headerStream << "--" << boundary << "\r\n"
                                 << "Content-Disposition: form-data; "
                                 << "name=\"audio\"; "
                                 << "filename=\"" <<std::filesystem::path(filePath).filename().string() << "\"\r\n"
                                 << "Content-Type: audio/mpeg\r\n"
                                 << "\r\n";
                    std::string header = headerStream.str();

                    std::string footer = "\r\n--" + boundary + "--\r\n";

                    // Reserve space
                    multipartData.reserve(header.size() + fileSize + footer.size());

                    // Add header
                    multipartData.insert(multipartData.end(), header.begin(), header.end());

                    // Add file content
                    multipartData.resize(multipartData.size() + fileSize);
                    file.read(multipartData.data() + header.size(), fileSize);
                    file.close();

                    // Add footer
                    multipartData.insert(multipartData.end(), footer.begin(), footer.end());

                    // Convert vector to string for easier handling
                    std::string bodyContent(multipartData.begin(), multipartData.end());

                    // SSL setup (same as before)
                    ssl::context sslContext(ssl::context::sslv23_client);
                    auto const results = _resolver.resolve(gladiapp::v2::common::HOST, "443");

                    beast::tcp_stream tcpStream(_ioContext);
                    tcpStream.connect(results);

                    ssl::stream<beast::tcp_stream> sslStream(std::move(tcpStream), sslContext);
                    sslStream.handshake(ssl::stream_base::client);

                    // Use buffer_body with the multipart data
                    http::request<http::string_body> httpRequest{
                        http::verb::post,
                        gladiapp::v2::common::UPLOAD_ENDPOINT,
                        11};
                    httpRequest.set(http::field::host, gladiapp::v2::common::HOST);
                    httpRequest.set(gladiapp::v2::headers::X_GLADIA_KEY, _apiKey);
                    httpRequest.set(http::field::user_agent, gladiapp::v2::common::USER_AGENT);
                    httpRequest.set(http::field::content_type, "multipart/form-data; boundary=" + boundary);
                    httpRequest.body() = bodyContent;
                    httpRequest.prepare_payload();

                    // Send using Beast's serialization
                    http::write(sslStream, httpRequest);

                    // Read response
                    beast::flat_buffer responseBuffer;
                    http::response<http::string_body> httpResponse;
                    http::read(sslStream, responseBuffer, httpResponse);

                    // Throw an exception if the response is not OK
                    if(httpResponse.result() != http::status::ok) {
                        std::ostringstream oss;
                        oss << "Upload failed, error code: " << (int)httpResponse.result()
                            << ", message: " << httpResponse.body();
                        spdlog::throw_spdlog_ex(oss.str(), (int)httpResponse.result());
                    }
                    return response::UploadResponse::fromJson(httpResponse.body());
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Error occurred: {}", e.what());
                }
                return response::UploadResponse();
            }

            response::UploadResponse preRecorded(const std::string &fileUrl)
            {
                return response::UploadResponse();
            }

        private:
            std::string _apiKey;
            net::io_context _ioContext;
            tcp::resolver _resolver;
        };
    }
}