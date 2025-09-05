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

/**************************************************************************************************************************************
 * GladiaWebsocketClient
 **************************************************************************************************************************************/

gladiapp::v2::ws::GladiaWebsocketClient::GladiaWebsocketClient(const std::string &apiKey) : _wsClientImpl(std::make_unique<GladiaWebsocketClientImpl>(apiKey))
{
}

gladiapp::v2::ws::GladiaWebsocketClient::~GladiaWebsocketClient()
{
}

GladiaWebsocketClientSession *gladiapp::v2::ws::GladiaWebsocketClient::connect(const request::InitializeSessionRequest &initRequest,
                                                                               gladiapp::v2::response::TranscriptionError *error)
{
    auto initSessionResponse = _wsClientImpl->connect(initRequest, error);
    if (error != nullptr && error->status_code != 0)
    {
        return nullptr;
    }
    return new GladiaWebsocketClientSession(initSessionResponse.url);
}

/**************************************************************************************************************************************
 * GladiaWebsocketClientSession
 **************************************************************************************************************************************/

 namespace gladiapp::v2::ws::response::events {
    constexpr const char* AUDIO_CHUNK_ACKNOWLEDGED = "audio_chunk_acknowledged";
    constexpr const char* STOP_RECORDING_ACKNOWLEDGED = "stop_recording_acknowledged";
    constexpr const char* START_SESSION = "start_session";
    constexpr const char* END_SESSION = "end_session";
    constexpr const char* START_RECORDING = "start_recording";
    constexpr const char* END_RECORDING = "end_recording";
 };

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
    return _wsClientSessionImpl->connectAndStart([this](const std::string &message)
                                                 {
                                                     nlohmann::json json = nlohmann::json::parse(message);
                                                     if (json.contains("type"))
                                                     {
                                                         std::string type = json["type"];
                                                         spdlog::info("Message type: {}", type);

                                                         if(type == "speech_start" ) {
                                                            response::SpeechStarted event = response::SpeechStarted::fromJson(json);
                                                            if (_onSpeechStartedCallback)
                                                            {
                                                                _onSpeechStartedCallback(event);
                                                            }
                                                         } else if(type == "speech_end") {
                                                            response::SpeechEnded event = response::SpeechEnded::fromJson(json);
                                                            if (_onSpeechEndedCallback)
                                                            {
                                                                _onSpeechEndedCallback(event);
                                                            }
                                                         } else if(type == "transcript") {
                                                            response::Transcript transcript = response::Transcript::fromJson(json);
                                                            if (_onTranscriptCallback)
                                                            {
                                                                _onTranscriptCallback(transcript);
                                                            }
                                                         } else if(type == "audio_chunk") {
                                                            bool isAck = json.value("acknowledged", false);
                                                         } else if(type == "translation") {
                                                            response::Translation translation = response::Translation::fromJson(json);
                                                            if (_onTranslationCallback)
                                                            {
                                                                _onTranslationCallback(translation);
                                                            }
                                                         }
                                                     } });
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

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnAudioChunkAcknowledgedCallback(const OnAudioChunkAcknowledgedCallback &callback)
{
    _onAudioChunkAcknowledgedCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnStopRecordingCallback(const OnStopRecordingAcknowledgmentCallback &callback)
{
    _onStopRecordingAcknowledgmentCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnConnectedCallback(const OnConnectedCallback &callback)
{
    _onConnectedCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnSpeechStartedCallback(const OnSpeechEventCallback &callback)
{
    _onSpeechStartedCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnSpeechEndedCallback(const OnSpeechEventCallback &callback)
{
    _onSpeechEndedCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnTranscriptCallback(const OnTranscriptCallback &callback)
{
    _onTranscriptCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnTranslationCallback(const OnTranslationCallback &callback)
{
    _onTranslationCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnNamedEntityRecognitionCallback(const OnNamedEntityRecognitionCallback &callback)
{
    _onNamedEntityRecognitionCallback = callback;
}
