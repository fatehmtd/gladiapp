#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>

namespace gladiapp
{
    namespace v2
    {
        namespace response
        {
            /**
             * Represents the response for an audio request.
             * Contains the audio file's URL and its metadata.
             */
            struct UploadResponse
            {
                std::string audio_url;

                /**
                 * Represents metadata about an audio file.
                 * Contains information such as the file's ID, filename, and other properties.
                 */
                struct AudioMetadata
                {
                    std::string id;
                    std::string filename;
                    std::string extension;
                    std::size_t size;
                    double audio_duration;
                    int number_of_channels;

                    static AudioMetadata fromJson(const std::string &jsonString);

                    std::string toString() const;
                };

                AudioMetadata audio_metadata;

                static UploadResponse fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents the response for a transcription job request.
             * Contains the transcription's ID and the URL to the result.
             */
            struct TranscriptionJobResponse
            {
                std::string id;
                std::string result_url;

                static TranscriptionJobResponse fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents an error that occurs during transcription.
             * Contains information about the error's timestamp, path, and message.
             * Additionally, it includes the request ID, status code, and any validation errors.
             */
            struct TranscriptionError
            {
                std::string timestamp;
                std::string path;
                std::string request_id;
                int status_code = 0;
                std::string message;
                std::vector<std::string> validation_errors;

                static TranscriptionError fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents a file to be transcribed.
             * Contains information about the file's ID and filename.
             * Additionally, it includes the file's source, duration, and number of channels.
             */
            struct TranscriptionFile
            {
                std::string id;
                std::string filename;
                std::string source;
                double duration;
                int number_of_channels;

                static TranscriptionFile fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            struct SentenceError
            {
                int status_code;
                std::string exception;
                std::string message;

                static SentenceError fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents the result of a transcription job.
             * Contains information about the job's ID, request ID, and other metadata.
             * Additionally, it includes the job's status, created_at timestamp, and any error information.
             */
            struct TranscriptionResult
            {
                std::string id;
                std::string request_id;
                int version;
                std::string status;
                std::string created_at;
                std::string kind;
                std::optional<std::string> completed_at;
                std::optional<int> error_code;
                std::optional<TranscriptionFile> file;
                std::string request_params;

                /**
                 * Represents the object containing the transcription result.
                 * Contains metadata and the actual transcription result.
                 */
                struct TranscriptionObject
                {
                    /**
                     * Represents the metadata for a transcription result.
                     * Contains information about the audio duration and number of distinct channels.
                     */
                    struct Metadata
                    {
                        double audio_duration;
                        int number_of_distinct_channels;
                        double billing_time;
                        double transcription_time;

                        static Metadata fromJson(const std::string &jsonString);
                    };
                    Metadata metadata;

                    /**
                     * Represents the result of a transcription job.
                     * Contains the full transcript and any detected languages.
                     */
                    struct Result
                    {
                        std::string full_transcript;
                        std::vector<std::string> languages;
                        struct Utterance
                        {
                            std::string language;
                            double start;
                            double end;
                            double confidence;
                            int channel;

                            struct Word
                            {
                                std::string word;
                                double start;
                                double end;
                                double confidence;

                                static Word fromJson(const std::string &jsonString);

                                std::string toString() const;
                            };

                            std::vector<Word> words;
                            std::string text;
                            int speaker;

                            static Utterance fromJson(const std::string &jsonString);

                            std::string toString() const;
                        };
                        std::vector<Utterance> utterances;

                        struct Subtitle
                        {
                            std::string format;
                            std::string subtitles;
                            static Subtitle fromJson(const std::string &jsonString);
                        };
                        std::vector<Subtitle> subtitles;

                        static Result fromJson(const std::string &jsonString);
                    };

                    Result result;

                    static TranscriptionObject fromJson(const std::string &jsonString);
                };
                TranscriptionObject result;

                struct Moderation
                {
                    bool success;
                    bool is_empty;
                    std::string results;
                    double exec_time;
                    std::optional<std::string> error;

                    static Moderation fromJson(const std::string &jsonString);
                };

                static TranscriptionResult fromJson(const std::string &jsonString);
            };

            /**
             * Represents the pagination information for a list of results.
             * Contains the URLs for the first, current, and next pages of results.
             */
            struct TranscriptionListResults
            {
                std::string first;
                std::string current;
                std::optional<std::string> next;
                std::vector<TranscriptionResult> items;
                static TranscriptionListResults fromJson(const std::string &jsonString);
            };
        }

        namespace request
        {
            /**
             * Represents information about an audio file.
             * Contains the audio file's URL and its metadata.
             */
            struct DiarizationConfig
            {
                int number_of_speakers;
                int min_speakers;
                int max_speakers;
                std::optional<bool> enhanced;
                nlohmann::json toJson() const;
            };

            /**
             * Represents the configuration for audio translation.
             * Contains the model to be used and the target languages for translation.
             */
            struct TranslationConfig
            {
                enum Model
                {
                    BASE,
                    ENHANCED
                };
                Model model = Model::BASE;
                std::vector<std::string> target_languages;
                std::optional<bool> match_original_utterances = true;
                std::optional<bool> lipsync = true;
                std::optional<bool> context_adaptation = true;
                std::optional<std::string> context;
                std::optional<bool> informal = false;

                nlohmann::json toJson() const;
            };

            /**
             * Represents the configuration for subtitles.
             * Contains the formats for the generated subtitles.
             */
            struct SubtitlesConfig
            {
                enum Format
                {
                    SRT,
                    VTT
                };
                std::vector<Format> formats;

                std::optional<int> maximum_characters_per_row;
                std::optional<int> maximum_rows_per_caption;

                enum Style
                {
                    DEFAULT,
                    COMPLIANCE
                };
                std::optional<Style> style = Style::DEFAULT;

                std::string toString() const;
            };

            /**
             * Represents the configuration for custom vocabulary.
             */
            struct CustomVocabularyConfig
            {
                std::string value;
                std::vector<std::string> pronunciations;
                std::optional<float> intensity;
                std::optional<std::string> language;
            };

            /**
             * Represents the configuration for callback settings.
             */
            struct CallbackConfig
            {
                std::string url;

                enum class HttpMethod
                {
                    POST,
                    PUT
                };
                /**
                 * HTTP method to use for the callback, only: POST, PUT are supported.
                 */
                HttpMethod method;

                std::string toJson() const;
            };

            /**
             * Represents the configuration for summarization.
             */
            struct SummarizationConfig
            {
                enum Type
                {
                    GENERAL,
                    BULLET_POINTS,
                    CONCISE
                };
                std::vector<Type> types;

                std::string toJson() const;
            };

            /**
             * Represents the configuration for custom spelling.
             */
            struct CustomSpellingConfig
            {
                std::unordered_map<std::string, std::vector<std::string>> spelling_dictionary;

                nlohmann::json toJson() const;
            };

            /**
             * Represents the configuration for structured data extraction.
             */
            struct StructuredDataExtractionConfig
            {
                std::vector<std::string> classes;

                nlohmann::json toJson() const;
            };

            /**
             * Represents the configuration for audio-to-LLM processing.
             */
            struct AudioToLLMConfig
            {
                std::vector<std::string> prompts;

                nlohmann::json toJson() const;
            };

            /**
             * Represents the configuration for language settings.
             */
            struct LanguageConfig
            {
                std::vector<std::string> languages;
                bool code_switching = false;

                nlohmann::json toJson() const;
            };

            /**
             * Represents the configuration for audio requests.
             * Contains the audio file's URL and its processing options.
             */
            struct TranscriptionRequest
            {
                std::string audio_url;

                bool customVocabulary = false;
                std::vector<CustomVocabularyConfig> custom_vocabulary_config;

                bool callback = false;
                std::optional<CallbackConfig> callback_config;

                bool subtitles = false;
                std::optional<SubtitlesConfig> subtitles_config;

                bool diarization = false;
                std::optional<DiarizationConfig> diarization_config;

                bool translation = false;
                TranslationConfig translation_config;

                bool summarization = false;
                std::optional<SummarizationConfig> summarization_config;

                bool moderation = false;

                bool named_entity_recognition = false;

                bool chapterization = false;

                bool name_consistency = false;

                bool custom_spelling = false;
                std::optional<CustomSpellingConfig> custom_spelling_config;

                bool structured_data_extraction = false;
                std::optional<StructuredDataExtractionConfig> structured_data_extraction_config;

                bool sentiment_analysis = false;

                bool audio_to_llm = false;
                std::optional<AudioToLLMConfig> audio_to_llm_config;

                bool sentences = false;

                bool display_mode = false;

                bool punctuation_enhanced = false;

                std::optional<LanguageConfig> language_config;

                nlohmann::json toJson() const;
            };

            struct ListResultsQuery
            {
                int offset = 0;
                int limit = 20;
                std::string date;
                std::string before_date;
                std::string after_date;
                enum Status
                {
                    QUEUED,
                    PROCESSING,
                    DONE,
                    ERROR
                };
                std::vector<Status> status;
                std::string toString() const;
            };
        }

        /**
         * forward declaration of the actual implementation
         */
        class GladiaRestClientImpl;

        class GladiaRestClient
        {
        public:
            GladiaRestClient() = delete;
            GladiaRestClient(const GladiaRestClient &) = delete;
            GladiaRestClient &operator=(const GladiaRestClient &) = delete;

            GladiaRestClient(const std::string &apiKey);
            ~GladiaRestClient();

            /**
             * Uploads an audio file for processing.
             * @param filePath The path to the audio file to upload.
             * @return The response from the server.
             */
            response::UploadResponse upload(const std::string &filePath) const;

            /**
             * Sends a pre-recorded audio url for processing.
             * @param transcriptionRequest The request containing the audio URL and other parameters.
             * @return The response from the server.
             */
            response::TranscriptionJobResponse preRecorded(const request::TranscriptionRequest &transcriptionRequest,
                                                           response::TranscriptionError *transcriptionError = nullptr) const;

            /**
             * Retrieves the result of a transcription job.
             * @param id The ID of the transcription job.
             * @return The response from the server.
             */
            response::TranscriptionResult getResult(const std::string &id,
                                                    response::TranscriptionError *transcriptionError = nullptr) const;

            /**
             * Retrieves the results of all transcription jobs.
             * @return A vector of responses from the server.
             */
            response::TranscriptionListResults getResults(const request::ListResultsQuery &query,
                                                          response::TranscriptionError *transcriptionError = nullptr) const;

        private:
            std::unique_ptr<GladiaRestClientImpl> _restClientImpl;
        };
    }
}