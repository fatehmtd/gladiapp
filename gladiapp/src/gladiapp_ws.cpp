#include "gladiapp_ws.hpp"
#include "gladiapp_ws_request.hpp"
#include "gladiapp_ws_response.hpp"
#include "impl/gladia_ws_client_boost_impl.hpp"

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
    if(_wsClientImpl == nullptr) {
        spdlog::error("WebSocket client implementation is not initialized.");
        return nullptr;
    }
    auto initSessionResponse = _wsClientImpl->connect(initRequest, error);
    return nullptr;
}

gladiapp::v2::ws::GladiaWebsocketClientSession::GladiaWebsocketClientSession(const std::string &url)
{
}

gladiapp::v2::ws::GladiaWebsocketClientSession::~GladiaWebsocketClientSession()
{
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::connectAndStart()
{
    return false;
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::sendStopSignal()
{
    return false;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::disconnect()
{
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::sendAudioBinary(const uint8_t *audioData, int size) const
{
    return false;
}

bool gladiapp::v2::ws::GladiaWebsocketClientSession::sendAudioJson(const uint8_t *audioData, int size) const
{
    return false;
}
