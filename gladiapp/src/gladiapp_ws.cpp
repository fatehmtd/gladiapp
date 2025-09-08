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
                                                 { this->processDataMessage(message); });
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::processDataMessage(const std::string &message) const
{
    try
    {
        nlohmann::json json = nlohmann::json::parse(message);
        if (json.contains("type"))
        {
            std::string type = json["type"];
            spdlog::info("Message type: {}", type);

            // Acknowledgment events
            if (type == events::AUDIO_CHUNK)
            {
                response::AudioChunkAcknowledgment ack = response::AudioChunkAcknowledgment::fromJson(json);
                if (_onAudioChunkAcknowledgedCallback)
                {
                    _onAudioChunkAcknowledgedCallback(ack);
                }
            }
            else if (type == events::STOP_RECORDING)
            {
                response::StopRecordingAcknowledgment ack = response::StopRecordingAcknowledgment::fromJson(json);
                if (_onStopRecordingAcknowledgmentCallback)
                {
                    _onStopRecordingAcknowledgmentCallback(ack);
                }
            }
            // Speech event types
            else if (type == events::SPEECH_START)
            {
                response::SpeechStarted event = response::SpeechStarted::fromJson(json);
                if (_onSpeechStartedCallback)
                {
                    _onSpeechStartedCallback(event);
                }
            }
            else if (type == events::SPEECH_END)
            {
                response::SpeechEnded event = response::SpeechEnded::fromJson(json);
                if (_onSpeechEndedCallback)
                {
                    _onSpeechEndedCallback(event);
                }
            }
            else if (type == events::TRANSCRIPT)
            {
                response::Transcript transcript = response::Transcript::fromJson(json);
                if (_onTranscriptCallback)
                {
                    _onTranscriptCallback(transcript);
                }
            }
            else if (type == events::TRANSLATION)
            {
                response::Translation translation = response::Translation::fromJson(json);
                if (_onTranslationCallback)
                {
                    _onTranslationCallback(translation);
                }
            }
            else if (type == events::NAMED_ENTITY_RECOGNITION)
            {
                response::NamedEntityRecognition ner = response::NamedEntityRecognition::fromJson(json);
                if (_onNamedEntityRecognitionCallback)
                {
                    _onNamedEntityRecognitionCallback(ner);
                }
            }
            else if (type == events::SENTIMENT_ANALYSIS)
            {
                response::SentimentAnalysis analysis = response::SentimentAnalysis::fromJson(json);
                if (_onSentimentAnalysisCallback)
                {
                    _onSentimentAnalysisCallback(analysis);
                }
            }
            // Post-processing event types
            else if (type == events::POST_TRANSCRIPTION)
            {
                response::PostTranscript postTranscript = response::PostTranscript::fromJson(json);
                if (_onPostTranscriptCallback)
                {
                    _onPostTranscriptCallback(postTranscript);
                }
            }
            else if (type == events::FINAL_TRANSCRIPTION)
            {
                response::FinalTranscript finalTranscript = response::FinalTranscript::fromJson(json);
                if (_onFinalTranscriptCallback)
                {
                    _onFinalTranscriptCallback(finalTranscript);
                }
            }
            else if (type == events::CHAPTERIZATION)
            {
                response::Chapterization chapterization = response::Chapterization::fromJson(json);
                if (_onChapterizationCallback)
                {
                    _onChapterizationCallback(chapterization);
                }
            }
            else if (type == events::SUMMARIZATION)
            {
                response::Summarization summarization = response::Summarization::fromJson(json);
                if (_onSummarizationCallback)
                {
                    _onSummarizationCallback(summarization);
                }
            }
            // Lifecycle event types
            else if (type == events::START_SESSION)
            {
                response::StartSession event = response::StartSession::fromJson(json);
                if (_onStartSessionCallback)
                {
                    _onStartSessionCallback(event);
                }
            }
            else if (type == events::END_SESSION)
            {
                response::EndSession event = response::EndSession::fromJson(json);
                if (_onEndSessionCallback)
                {
                    _onEndSessionCallback(event);
                }
            }
            else if (type == events::START_RECORDING)
            {
                response::StartRecording event = response::StartRecording::fromJson(json);
                if (_onStartRecordingCallback)
                {
                    _onStartRecordingCallback(event);
                }
            }
            else if (type == events::END_RECORDING)
            {
                response::EndRecording event = response::EndRecording::fromJson(json);
                if (_onEndRecordingCallback)
                {
                    _onEndRecordingCallback(event);
                }
            }
            else
            {
                spdlog::warn("Unknown event type received: {}", type);
            }
        }
        else
        {
            spdlog::warn("Received message without 'type' field: {}", message);
        }
    }
    catch (const nlohmann::json::exception &e)
    {
        spdlog::error("Failed to parse JSON message: {}. Error: {}", message, e.what());
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error processing message: {}. Error: {}", message, e.what());
    }
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

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnPostTranscriptCallback(const OnPostTranscriptCallback &callback)
{
    _onPostTranscriptCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnFinalTranscriptCallback(const OnFinalTranscriptCallback &callback)
{
    _onFinalTranscriptCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnChapterizationCallback(const OnChapterizationCallback &callback)
{
    _onChapterizationCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnSummarizationCallback(const OnSummarizationCallback &callback)
{
    _onSummarizationCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnAudioChunkAcknowledgedCallback(const OnAudioChunkAcknowledgedCallback &callback)
{
    _onAudioChunkAcknowledgedCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnStopRecordingCallback(const OnStopRecordingAcknowledgmentCallback &callback)
{
    _onStopRecordingAcknowledgmentCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnStartSessionCallback(const OnStartSessionCallback &callback)
{
    _onStartSessionCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnStartRecordingCallback(const OnStartRecordingCallback &callback)
{
    _onStartRecordingCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnEndRecordingCallback(const OnEndRecordingCallback &callback)
{
    _onEndRecordingCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnEndSessionCallback(const OnEndSessionCallback &callback)
{
    _onEndSessionCallback = callback;
}

void gladiapp::v2::ws::GladiaWebsocketClientSession::setOnConnectedCallback(const OnConnectivityCallback &callback)
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
