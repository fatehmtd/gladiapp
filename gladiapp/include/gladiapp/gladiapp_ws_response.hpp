#pragma once
#include "gladiapp_export.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "gladiapp_error.hpp"
#include "gladiapp_rest_response.hpp"
#include "gladiapp_ws_request.hpp"

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
                    std::optional<int> status_code;
                    std::optional<std::string> exception;
                    std::optional<std::string> message;

                    static Error fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a word in the utterance.
                 */
                struct GLADIAPP_EXPORT Word
                {
                    std::string word;
                    double start;
                    double end;
                    double confidence;

                    static Word fromJson(const nlohmann::json &json);
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
                 * Represents a sentence in the transcription.
                 */
                struct GLADIAPP_EXPORT Sentence
                {
                    bool success;
                    bool is_empty;
                    double exec_time;
                    std::optional<Error> error;
                    std::optional<std::vector<std::string>> results;

                    static Sentence fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents subtitles in various formats.
                 */
                struct GLADIAPP_EXPORT Subtitle
                {
                    std::string format;
                    std::string subtitles;
                    static Subtitle fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents Metadata.
                 */
                struct GLADIAPP_EXPORT Metadata
                {
                    double audio_duration;
                    int number_of_distinct_channels;
                    double billing_time;
                    double transcription_time;
                    static Metadata fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents the transcription event after post-processing.
                 */
                struct GLADIAPP_EXPORT Transcription
                {
                    std::string full_transcript;
                    std::vector<std::string> languages;
                    std::vector<Subtitle> subtitles;
                    std::vector<Utterance> utterances;

                    /**
                     * generic format of various post-processing results
                     */
                    struct GenericResult
                    {
                        bool success;
                        bool is_empty;
                        double exec_time;
                        std::optional<Error> error;
                        std::vector<std::string> results;

                        static GenericResult fromJson(const nlohmann::json &json);
                    };

                    using SummarizationResult = GenericResult;
                    std::optional<SummarizationResult> summarization;

                    using ModerationResult = GenericResult;
                    std::optional<ModerationResult> moderation;

                    /**
                     * Named entity recognition result structure.
                     */
                    struct NamedEntityRecognitionResult
                    {
                        bool success;
                        bool is_empty;
                        double exec_time;

                        std::optional<Error> error;
                        std::optional<std::string> entity;

                        static NamedEntityRecognitionResult fromJson(const nlohmann::json &json);
                    };
                    std::optional<NamedEntityRecognitionResult> named_entity_recognition;

                    using NameConsistencyResult = GenericResult;
                    std::optional<NameConsistencyResult> name_consistency;

                    using CustomSpellingResult = GenericResult;
                    std::optional<CustomSpellingResult> custom_spelling;

                    using SpeakerReidentificationResult = GenericResult;
                    std::optional<SpeakerReidentificationResult> speaker_reidentification;

                    using StructuredDataExtractionResult = GenericResult;
                    std::optional<StructuredDataExtractionResult> structured_data_extraction;

                    using SentimentAnalysisResult = GenericResult;
                    std::optional<SentimentAnalysisResult> sentiment_analysis;

                    /**
                     * Audio to LLM result structure.
                     */
                    struct AudioToLLMResult
                    {
                        bool success;
                        bool is_empty;
                        double exec_time;

                        std::optional<Error> error;

                        struct Result
                        {
                            bool success;
                            bool is_empty;
                            double exec_time;
                            std::optional<Error> error;

                            struct ResultPair
                            {
                                std::optional<std::string> prompt;
                                std::optional<std::string> response;
                            };
                            std::optional<std::vector<ResultPair>> results;

                            static Result fromJson(const nlohmann::json &json);
                        };
                        std::optional<std::vector<Result>> results;

                        static AudioToLLMResult fromJson(const nlohmann::json &json);
                    };
                    std::optional<AudioToLLMResult> audio_to_llm;

                    /**
                     * Display mode result structure.
                     */
                    struct DisplayMode
                    {
                        bool success;
                        bool is_empty;
                        double exec_time;
                        std::optional<Error> error;
                        std::optional<std::vector<std::string>> results;

                        static DisplayMode fromJson(const nlohmann::json &json);
                    };
                    std::optional<DisplayMode> display_mode;

                    /**
                     * Chapterization result structure.
                     */
                    struct ChapterizationResult
                    {
                        bool success;
                        bool is_empty;
                        double exec_time;
                        std::optional<Error> error;

                        // undefined format of the results, missing info from the api reference.
                        std::optional<std::string> results;

                        static ChapterizationResult fromJson(const nlohmann::json &json);
                    };
                    std::optional<ChapterizationResult> chapters;

                    /**
                     * Diarization result structure.
                     */
                    struct DiarizationResult
                    {
                        bool success;
                        bool is_empty;
                        double exec_time;
                        std::optional<Error> error;

                        struct Result
                        {
                            double start;
                            double end;
                            double confidence;
                            int channel;
                            std::optional<int> speaker;
                            std::vector<Word> words;
                            std::string text;
                            std::string language;

                            static Result fromJson(const nlohmann::json &json);
                        };
                        std::optional<std::vector<Result>> results;

                        static DiarizationResult fromJson(const nlohmann::json &json);
                    };
                    using DiarizationEnhancedResult = DiarizationResult;
                    std::optional<DiarizationEnhancedResult> diarization_enhanced;
                    std::optional<DiarizationResult> diarization;

                    static Transcription fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a post-transcript event.
                 */
                struct GLADIAPP_EXPORT PostTranscript
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data
                    {
                        std::string full_transcript;
                        std::vector<std::string> languages;
                        std::vector<Utterance> utterances;
                        std::vector<Sentence> sentences;
                        std::optional<std::vector<std::string>> results;
                        std::optional<std::vector<Subtitle>> subtitles;
                    };
                    Data data;

                    static PostTranscript fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents the final script event.
                 */
                struct GLADIAPP_EXPORT FinalTranscript
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data
                    {
                        Metadata metadata;
                        std::optional<Transcription> transcription;
                        std::optional<Translation> translation;
                    };
                    Data data;

                    static FinalTranscript fromJson(const nlohmann::json &json);
                };

                /**
                 * Summarization event structure.
                 */
                struct GLADIAPP_EXPORT Summarization
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data
                    {
                        std::string results;
                    };
                    std::optional<Data> data;
                    static Summarization fromJson(const nlohmann::json &json);
                };

                /**
                 * Sentiment analysis event structure.
                 */
                struct GLADIAPP_EXPORT SentimentAnalysis
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    struct Data
                    {
                        std::string utterance_id;
                        Utterance utterance;
                        struct Result
                        {
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

                struct GLADIAPP_EXPORT Chapterization
                {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    std::optional<Error> error;
                    struct Data
                    {
                        struct Chapter
                        {
                            std::string headline;
                            std::string gist;
                            std::vector<std::string> keywords;
                            double start;
                            double end;
                            std::vector<Sentence> sentences;
                            std::string text;
                            std::string abstractive_summary;
                            std::string extractive_summary;
                            std::string summary;

                            static Chapter fromJson(const nlohmann::json &json);
                        };
                        std::vector<Chapter> results;
                    };
                    std::optional<Data> data;
                    static Chapterization fromJson(const nlohmann::json &json);
                };

                /**
                 * Lifecycle event structure.
                 */
                struct GLADIAPP_EXPORT LifecycleEvent
                {
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
                 * Audio chunk acknowledgment event structure.
                 */
                struct GLADIAPP_EXPORT AudioChunkAcknowledgment {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    bool acknowledged;
                    std::optional<Error> error;
                    struct Data {
                        std::vector<double> byte_range;
                        std::vector<double> time_range;
                    };
                    std::optional<Data> data;
                    static AudioChunkAcknowledgment fromJson(const nlohmann::json &json);
                };

                /**
                 * Stop recording acknowledgment event structure.
                 */
                struct GLADIAPP_EXPORT StopRecordingAcknowledgment {
                    std::string session_id;
                    std::string created_at;
                    std::string type;
                    bool acknowledged;
                    std::optional<Error> error;
                    struct Data {
                        double recording_duration;
                        double recording_left_to_process;
                    };
                    std::optional<Data> data;
                    static StopRecordingAcknowledgment fromJson(const nlohmann::json &json);
                };

                /**
                 * Represents a Live Transcription Result.
                 */
                struct GLADIAPP_EXPORT LiveTranscriptionResult
                {
                    std::string id;
                    std::string request_id;
                    int version;
                    std::string status;
                    std::string created_at;
                    std::optional<std::string> completed_at;
                    std::string kind;
                    std::optional<std::string> custom_metadata;
                    std::optional<int> error_code;

                    gladiapp::v2::response::TranscriptionFile file;

                    using RequestParams = gladiapp::v2::ws::request::InitializeSessionRequest;
                    RequestParams request_params;                

                    struct Result {
                        Metadata metadata;
                        std::vector<std::string> messages;
                        std::optional<Transcription> transcription;

                        struct TranslationResult {
                            bool success;
                            bool is_empty;
                            double exec_time;
                            std::optional<Error> error;
                            struct Result {
                                std::optional<Error> error;
                                std::string full_transcript;
                                std::vector<std::string> languages;
                                std::vector<Utterance> utterances;
                                std::optional<std::vector<Sentence>> sentences;
                                std::optional<std::vector<Subtitle>> subtitles;

                                static Result fromJson(const nlohmann::json &json);
                            };
                            std::optional<std::vector<Result>> results;

                            static TranslationResult fromJson(const nlohmann::json &json);
                        };
                        std::optional<TranslationResult> translation;

                        std::optional<Transcription::SummarizationResult> summarization;
                        std::optional<Transcription::ModerationResult> moderation;
                        std::optional<Transcription::NamedEntityRecognitionResult> named_entity_recognition;
                        std::optional<Transcription::NameConsistencyResult> name_consistency;
                        std::optional<Transcription::CustomSpellingResult> custom_spelling;
                        std::optional<Transcription::SpeakerReidentificationResult> speaker_reidentification;
                        std::optional<Transcription::StructuredDataExtractionResult> structured_data_extraction;
                        std::optional<Transcription::SentimentAnalysisResult> sentiment_analysis;
                        std::optional<Transcription::AudioToLLMResult> audio_to_llm;
                        std::optional<Transcription::DisplayMode> display_mode;
                        std::optional<Transcription::ChapterizationResult> chapters;
                        std::optional<Transcription::DiarizationEnhancedResult> diarization_enhanced;
                        std::optional<Transcription::DiarizationResult> diarization;

                        static Result fromJson(const nlohmann::json &json);
                    };
                    Result result;
                    
                    static LiveTranscriptionResult fromJson(const nlohmann::json &json);
                };
            }
        }
    }
}