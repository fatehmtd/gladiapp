#pragma once

#include "gladiapp_export.h"
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "gladiapp_error.hpp"
#include "gladiapp_ws_request.hpp"
#include "gladiapp_ws_response.hpp"

namespace gladiapp
{
    namespace v2
    {
        namespace ws
        {
            // Forward declaration for the WebSocket client session
            class GladiaWebsocketClientSession;

            // Forward declaration for the implementation details
            class GladiaWebsocketClientImpl;

            /**
             * WebSocket client for Gladia API.
             */
            class GLADIAPP_EXPORT GladiaWebsocketClient
            {
            public:
                GladiaWebsocketClient(const GladiaWebsocketClient &) = delete;
                GladiaWebsocketClient &operator=(const GladiaWebsocketClient &) = delete;

                GladiaWebsocketClient(const std::string &apiKey);
                ~GladiaWebsocketClient();

                GladiaWebsocketClientSession *connect(const request::InitializeSessionRequest &initRequest,
                                                      gladiapp::v2::response::TranscriptionError *error = nullptr);

            private:
                std::unique_ptr<GladiaWebsocketClientImpl> _wsClientImpl;
            };

            // Forward declaration for the implementation details
            class GladiaWebsocketClientSessionImpl;

            /**
             * WebSocket client session for Gladia API.
             */
            class GLADIAPP_EXPORT GladiaWebsocketClientSession
            {
            public:
                // Deleted copy constructor and assignment operator
                GladiaWebsocketClientSession(const GladiaWebsocketClientSession &) = delete;
                GladiaWebsocketClientSession &operator=(const GladiaWebsocketClientSession &) = delete;

                GladiaWebsocketClientSession(const std::string &url);
                ~GladiaWebsocketClientSession();

                /**
                 * Connects to the WebSocket server and starts the session and the data reception thread.
                 */
                bool connectAndStart();

                /**
                 * Sends the stop signal.
                 */
                bool sendStopSignal();

                /**
                 * Disconnects from the WebSocket server.
                 */
                void disconnect();

                /**
                 * Sends audio data as binary to the WebSocket server.
                 */
                bool sendAudioBinary(const uint8_t *audioData, int size) const;

                /**
                 * Sends audio data as base64-encoded JSON to the WebSocket server.
                 */
                bool sendAudioJson(const uint8_t *audioData, int size) const;

                using OnConnectedCallback = std::function<void(void)>;
                using OnSpeechEventCallback = std::function<void(const response::SpeechEvent &event)>;
                using OnTranscriptCallback = std::function<void(const response::Transcript &transcript)>;
                using OnTranslationCallback = std::function<void(const response::Translation &translation)>;
                using OnNamedEntityRecognitionCallback = std::function<void(const response::NamedEntityRecognition &ner)>;

                using OnSentimentAnalysisCallback = std::function<void(const response::SentimentAnalysis &analysis)>;
                using OnSummarizationCallback = std::function<void(const response::Summarization &summarization)>;

                /**
                 * Acknowledgment callbacks
                 */
                using OnAudioChunkAcknowledgedCallback = std::function<void(const response::AudioChunkAcknowledgment &acknowledged)>;
                using OnStopRecordingAcknowledgmentCallback = std::function<void(const response::StopRecordingAcknowledgment &event)>;

                void setOnAudioChunkAcknowledgedCallback(const OnAudioChunkAcknowledgedCallback &callback);
                void setOnStopRecordingCallback(const OnStopRecordingAcknowledgmentCallback &callback);

                /**
                 * Lifecycle event callbacks
                 */
                using OnStartSessionCallback = std::function<void(const response::StartSession &event)>;
                using OnEndSessionCallback = std::function<void(const response::EndSession &event)>;
                using OnStartRecordingCallback = std::function<void(const response::StartRecording &event)>;
                using OnEndRecordingCallback = std::function<void(const response::EndRecording &event)>;

                void setOnConnectedCallback(const OnConnectedCallback &callback);
                void setOnSpeechStartedCallback(const OnSpeechEventCallback &callback);
                void setOnSpeechEndedCallback(const OnSpeechEventCallback &callback);
                void setOnTranscriptCallback(const OnTranscriptCallback &callback);
                void setOnTranslationCallback(const OnTranslationCallback &callback);
                void setOnNamedEntityRecognitionCallback(const OnNamedEntityRecognitionCallback &callback);

            private:
                std::unique_ptr<GladiaWebsocketClientSessionImpl> _wsClientSessionImpl;
                OnConnectedCallback _onConnectedCallback;
                OnSpeechEventCallback _onSpeechStartedCallback;
                OnSpeechEventCallback _onSpeechEndedCallback;
                OnTranscriptCallback _onTranscriptCallback;
                OnTranslationCallback _onTranslationCallback;
                OnNamedEntityRecognitionCallback _onNamedEntityRecognitionCallback;
                OnSentimentAnalysisCallback _onSentimentAnalysisCallback;
                OnSummarizationCallback _onSummarizationCallback;
                OnAudioChunkAcknowledgedCallback _onAudioChunkAcknowledgedCallback;
                OnStopRecordingAcknowledgmentCallback _onStopRecordingAcknowledgmentCallback;
                OnStartSessionCallback _onStartSessionCallback;
                OnEndSessionCallback _onEndSessionCallback;
                OnStartRecordingCallback _onStartRecordingCallback;
                OnEndRecordingCallback _onEndRecordingCallback;
            };
        }
    }
}