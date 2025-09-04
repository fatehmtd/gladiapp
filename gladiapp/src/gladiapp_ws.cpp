#include "gladiapp_ws.hpp"
#include "gladiapp_ws_request.hpp"
#include "gladiapp_ws_response.hpp"
#include "impl/gladia_ws_client_boost_impl.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <base64.hpp>

using namespace gladiapp::v2::ws::response;
using namespace gladiapp::v2::ws::request;
using namespace gladiapp::v2::ws;

gladiapp::v2::ws::GladiaWebsocketClient::GladiaWebsocketClient(const std::string &apiKey) : _wsClientImpl(std::make_unique<GladiaWebsocketClientImpl>(apiKey))
{
}

gladiapp::v2::ws::GladiaWebsocketClient::~GladiaWebsocketClient()
{
}

GladiaWebsocketClientSession *gladiapp::v2::ws::GladiaWebsocketClient::connect(const request::InitializeSessionRequest &initRequest,
                                                                               gladiapp::v2::response::TranscriptionError *error)
{
    if (_wsClientImpl == nullptr)
    {
        spdlog::error("WebSocket client implementation is not initialized.");
        return nullptr;
    }
    auto initSessionResponse = _wsClientImpl->connect(initRequest, error);
    if (error != nullptr && error->status_code != 0)
    {
        spdlog::error("Error initializing session: {}", error->toString());
        return nullptr;
    }
    return new GladiaWebsocketClientSession(initSessionResponse.url);
}

/**************************************************************************************************************************************
 * GladiaWebsocketClientSession
 **************************************************************************************************************************************/

gladiapp::v2::ws::GladiaWebsocketClientSession::GladiaWebsocketClientSession(const std::string &url) : _wsClientSessionImpl(std::make_unique<GladiaWebsocketClientSessionImpl>(url))
{
}

gladiapp::v2::ws::GladiaWebsocketClientSession::~GladiaWebsocketClientSession()
{
    sendStopSignal();
    disconnect();
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::connectAndStart()
{
    if (_wsClientSessionImpl->isConnected())
    {
        spdlog::warn("WebSocket is already connected.");
        return true;
    }
    return _wsClientSessionImpl->connectAndStart([](const std::string &message)
                                                 { spdlog::info("Received message: {}", message); });
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::sendStopSignal()
{
    if (!_wsClientSessionImpl->isConnected())
    {
        spdlog::warn("WebSocket is not connected. Cannot send stop signal.");
        return false;
    }
    nlohmann::json stopJson;
    stopJson["type"] = "stop_recording";
    return _wsClientSessionImpl->sendTextJson(stopJson.dump());
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::disconnect()
{
    if (!_wsClientSessionImpl->isConnected())
    {
        spdlog::warn("WebSocket is not connected.");
        return;
    }
    _wsClientSessionImpl->disconnect();
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::sendAudioBinary(const uint8_t *audioData, int size) const
{
    if (!_wsClientSessionImpl->isConnected())
    {
        spdlog::warn("WebSocket is not connected. Cannot send audio binary.");
        return false;
    }
    return _wsClientSessionImpl->sendAudioBinary(audioData, size);
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::sendAudioJson(const uint8_t *audioData, int size) const
{
    if (!_wsClientSessionImpl->isConnected())
    {
        spdlog::warn("WebSocket is not connected. Cannot send audio JSON.");
        return false;
    }
    nlohmann::json audioJson;
    audioJson["type"] = "audio_chunk";
    std::string binaryData(reinterpret_cast<const char *>(audioData), size);
    nlohmann::json chunk;
    chunk["chunk"] = base64::to_base64(binaryData);
    audioJson["data"] = chunk;
    return _wsClientSessionImpl->sendTextJson(audioJson.dump());
}
