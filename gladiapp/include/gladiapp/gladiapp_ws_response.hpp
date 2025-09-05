#pragma once
#include "gladiapp_export.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace gladiapp
{
    namespace v2
    {
        namespace ws
        {
            namespace response
            {
                /** Represents the response for initializing a WebSocket session. */
                struct GLADIAPP_EXPORT InitializeSessionResponse
                {
                    std::string id;
                    std::string url;
                    static InitializeSessionResponse fromJson(const nlohmann::json &json);
                };

                /** Represents a speech event. */
                struct GLADIAPP_EXPORT SpeechEvent
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    struct Data
                    {
                        double time;
                        int channel;
                    };
                    Data data;

                    static SpeechEvent fromJson(const nlohmann::json &json);
                };

                // Type aliases for speech events
                using SpeechStarted = SpeechEvent;
                using SpeechEnded = SpeechEvent;

                /**
                 * Common error structure.
                 */
                struct GLADIAPP_EXPORT Error
                {
                    std::string message;

                    static Error fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents an utterance
                 */
                struct GLADIAPP_EXPORT Utterance
                {
                    std::string language;
                    double start;
                    double end;
                    double confidence;
                    int channel;
                    /**
                     * Represents a word in the utterance.
                     */
                    struct Word
                    {
                        std::string word;
                        double start;
                        double end;
                        double confidence;

                        static Word fromJson(const nlohmann::json &json);
                    };
                    std::vector<Word> words;
                    std::string text;
                    std::optional<int> speaker;

                    static Utterance fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a transcript event.
                 */
                struct GLADIAPP_EXPORT Transcript
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    struct Data
                    {
                        std::string id;
                        bool is_final;
                        Utterance utterance;
                    };
                    Data data;

                    static Transcript fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a translation event.
                 */
                struct GLADIAPP_EXPORT Translation
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data
                    {
                        std::string utterance_id;
                        Utterance utterance;
                        std::string original_language;
                        std::string target_language;
                        Utterance translated_utterance;
                    };
                    std::optional<Data> data;

                    static Translation fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a named entity recognition event.
                 */
                struct GLADIAPP_EXPORT NamedEntityRecognition
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data
                    {
                        std::string utterance_id;
                        Utterance utterance;
                        struct Result
                        {
                            std::string entity_type;
                            std::string text;
                            double start;
                            double end;
                            static Result fromJson(const nlohmann::json &json);
                        };
                        std::vector<Result> results;
                    };
                    std::optional<Data> data;

                    static NamedEntityRecognition fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a post-transcript event.
                 */
                class GLADIAPP_EXPORT PostTranscript {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    struct Data {
                        std::string full_transcript;
                        std::vector<std::string> languages;
                        std::vector<Utterance> utterances;
                        struct Sentence {
                            bool success;
                            bool is_empty;
                            double exec_time;
                            struct Error {
                                int status_code;
                                std::string exception;
                                std::string message;
                            };
                            std::optional<Error> error;

                            static Sentence fromJson(const nlohmann::json &json);
                        };
                        std::vector<Sentence> sentences;
                        std::optional<std::vector<std::string>> results;
                        struct Subtitle {
                            std::string format;
                            std::string subtitles;
                            static Subtitle fromJson(const nlohmann::json &json);
                        };
                        std::optional<std::vector<Subtitle>> subtitles;
                    };
                    Data data;

                    static PostTranscript fromJson(const nlohmann::json &json);
                };

                /**
                 * Summarization event structure.
                 */
                struct GLADIAPP_EXPORT Summarization {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data {
                        std::string results;
                    };
                    std::optional<Data> data;
                    static Summarization fromJson(const nlohmann::json &json);
                };

            
                /**
                 * Sentiment analysis event structure.
                 */
                struct GLADIAPP_EXPORT SentimentAnalysis {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    struct Data {
                        std::string utterance_id;
                        Utterance utterance;
                        struct Result {
                            std::string sentiment;
                            std::string emotion;
                            std::string text;
                            double start;
                            double end;
                            double channel;

                            static Result fromJson(const nlohmann::json &json);
                        };
                        std::vector<Result> results;
                    };
                    Data data;

                    static SentimentAnalysis fromJson(const nlohmann::json &json);
                };

                /**
                 * Lifecycle event structure.
                 */
                struct GLADIAPP_EXPORT LifecycleEvent {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    static LifecycleEvent fromJson(const nlohmann::json &json);
                };

                using StartSession = LifecycleEvent;
                using EndSession = LifecycleEvent;
                using StartRecording = LifecycleEvent;
                using EndRecording = LifecycleEvent;

                /**
                 * Acknowledgment event structure.
                 */
                struct GLADIAPP_EXPORT Acknowledgment {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    bool acknowledged;
                    static Acknowledgment fromJson(const nlohmann::json &json);
                };

                using AudioChunkAcknowledgment = Acknowledgment;
                using StopRecordingAcknowledgment = Acknowledgment;
            }
        }
    }
}