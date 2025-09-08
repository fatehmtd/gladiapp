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
            /**
             * Event type constants
             */
            namespace events
            {
                /**
                 * acknowledgment types
                 */
                constexpr const char *AUDIO_CHUNK = "audio_chunk";
                constexpr const char *STOP_RECORDING = "stop_recording";

                /**
                 * speech event types
                 */
                constexpr const char *SPEECH_START = "speech_start";
                constexpr const char *SPEECH_END = "speech_end";
                constexpr const char *TRANSCRIPT = "transcript";
                constexpr const char *TRANSLATION = "translation";
                constexpr const char *NAMED_ENTITY_RECOGNITION = "named_entity_recognition";
                constexpr const char *SENTIMENT_ANALYSIS = "sentiment_analysis";

                /**
                 * Post-processing event types
                 */
                constexpr const char *POST_TRANSCRIPTION = "post_transcript";
                constexpr const char *FINAL_TRANSCRIPTION = "post_final_transcript";
                constexpr const char *CHAPTERIZATION = "post_chapterization";
                constexpr const char *SUMMARIZATION = "post_summarization";

                /**
                 * lifecycle event types
                 */
                constexpr const char *START_SESSION = "start_session";
                constexpr const char *END_SESSION = "end_session";
                constexpr const char *START_RECORDING = "start_recording";
                constexpr const char *END_RECORDING = "end_recording";
            };

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

                /**
                 * Connectivity callbacks
                 */
                using OnConnectivityCallback = std::function<void(void)>;
                using OnErrorCallback = std::function<void(const std::string &errorMessage)>;

                void setOnConnectedCallback(const OnConnectivityCallback &callback);
                void setOnDisconnectedCallback(const OnConnectivityCallback &callback);
                void setOnErrorCallback(const OnErrorCallback &callback);

                /**
                 * Speech event callbacks
                 */
                using OnSpeechEventCallback = std::function<void(const response::SpeechEvent &event)>;
                using OnTranscriptCallback = std::function<void(const response::Transcript &transcript)>;
                using OnTranslationCallback = std::function<void(const response::Translation &translation)>;
                using OnNamedEntityRecognitionCallback = std::function<void(const response::NamedEntityRecognition &ner)>;
                using OnSentimentAnalysisCallback = std::function<void(const response::SentimentAnalysis &analysis)>;

                void setOnSpeechStartedCallback(const OnSpeechEventCallback &callback);
                void setOnSpeechEndedCallback(const OnSpeechEventCallback &callback);
                void setOnTranscriptCallback(const OnTranscriptCallback &callback);
                void setOnTranslationCallback(const OnTranslationCallback &callback);
                void setOnNamedEntityRecognitionCallback(const OnNamedEntityRecognitionCallback &callback);
                void setOnSentimentAnalysisCallback(const OnSentimentAnalysisCallback &callback);

                /**
                 * Post-processing event callbacks
                 */
                using OnPostTranscriptCallback = std::function<void(const response::PostTranscript &postTranscript)>;
                using OnFinalTranscriptCallback = std::function<void(const response::FinalTranscript &finalScript)>;
                using OnChapterizationCallback = std::function<void(const response::Chapterization &chapterization)>;
                using OnSummarizationCallback = std::function<void(const response::Summarization &summarization)>;

                void setOnPostTranscriptCallback(const OnPostTranscriptCallback &callback);
                void setOnFinalTranscriptCallback(const OnFinalTranscriptCallback &callback);
                void setOnChapterizationCallback(const OnChapterizationCallback &callback);
                void setOnSummarizationCallback(const OnSummarizationCallback &callback);

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
                
                void setOnStartSessionCallback(const OnStartSessionCallback &callback);
                void setOnEndSessionCallback(const OnEndSessionCallback &callback);
                void setOnStartRecordingCallback(const OnStartRecordingCallback &callback);
                void setOnEndRecordingCallback(const OnEndRecordingCallback &callback);

            private:
                std::unique_ptr<GladiaWebsocketClientSessionImpl> _wsClientSessionImpl;
                /**
                 * Callback functions
                 */
                OnConnectivityCallback _onConnectedCallback;
                OnConnectivityCallback _onDisconnectedCallback;
                OnErrorCallback _onErrorCallback;

                /**
                 * Speech event callbacks
                 */
                OnSpeechEventCallback _onSpeechStartedCallback;
                OnSpeechEventCallback _onSpeechEndedCallback;
                OnTranscriptCallback _onTranscriptCallback;
                OnTranslationCallback _onTranslationCallback;
                OnNamedEntityRecognitionCallback _onNamedEntityRecognitionCallback;
                OnSentimentAnalysisCallback _onSentimentAnalysisCallback;

                /**
                 * Post-processing event callbacks
                 */
                OnSummarizationCallback _onSummarizationCallback;
                OnFinalTranscriptCallback _onFinalTranscriptCallback;
                OnPostTranscriptCallback _onPostTranscriptCallback;
                OnChapterizationCallback _onChapterizationCallback;

                /**
                 * Acknowledgment callbacks
                 */
                OnAudioChunkAcknowledgedCallback _onAudioChunkAcknowledgedCallback;
                OnStopRecordingAcknowledgmentCallback _onStopRecordingAcknowledgmentCallback;

                /**
                 * Lifecycle event callbacks
                 */
                OnStartSessionCallback _onStartSessionCallback;
                OnEndSessionCallback _onEndSessionCallback;
                OnStartRecordingCallback _onStartRecordingCallback;
                OnEndRecordingCallback _onEndRecordingCallback;

                // Process incoming WebSocket messages
                void processDataMessage(const std::string &message) const;
            };
        }
    }
}