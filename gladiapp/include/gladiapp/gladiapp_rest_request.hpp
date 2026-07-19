#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <memory>

#include "gladiapp_export.h"
#include "gladiapp_error.hpp"

namespace gladiapp
{
    namespace v2
    {
        namespace request
        {
            /**
             * Represents the configuration for audio requests.
             * Contains the audio file's URL and its processing options.
             */
            struct GLADIAPP_EXPORT TranscriptionRequest
            {
                std::string audio_url;

                bool custom_vocabulary = false;
                /**
                 * Represents the configuration for custom vocabulary.
                 */
                struct CustomVocabularyConfig
                {
                    /**
                     * Represents a single vocabulary entry.
                     * Contains the word, its intensity, and possible pronunciations.
                     */
                    struct Vocabulary
                    {
                        std::string value;
                        std::vector<std::string> pronunciations;
                        std::optional<float> intensity;
                        std::optional<std::string> language;

                        nlohmann::json toJson() const;
                    };
                    std::vector<Vocabulary> vocabulary;
                    /**
                     * Default intensity for the custom vocabulary, between 0 and 1.
                     */
                    std::optional<float> default_intensity;

                    nlohmann::json toJson() const;
                };
                std::optional<CustomVocabularyConfig> custom_vocabulary_config;

                bool callback = false;
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
                std::optional<CallbackConfig> callback_config;

                bool subtitles = false;
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

                    /**
                     * Minimum duration of a subtitle in seconds.
                     */
                    std::optional<double> minimum_duration;
                    /**
                     * Maximum duration of a subtitle in seconds, between 1 and 30.
                     */
                    std::optional<double> maximum_duration;
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
                std::optional<SubtitlesConfig> subtitles_config;

                bool diarization = false;
                /**
                 * Represents the configuration for speaker diarization.
                 */
                struct DiarizationConfig
                {
                    std::optional<int> number_of_speakers;
                    std::optional<int> min_speakers;
                    std::optional<int> max_speakers;
                    nlohmann::json toJson() const;
                };
                std::optional<DiarizationConfig> diarization_config;

                bool translation = false;
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
                TranslationConfig translation_config;

                bool summarization = false;
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
                    Type type = Type::GENERAL;

                    nlohmann::json toJson() const;
                };
                std::optional<SummarizationConfig> summarization_config;

                bool named_entity_recognition = false;

                bool custom_spelling = false;
                /**
                 * Represents the configuration for custom spelling.
                 */
                struct CustomSpellingConfig
                {
                    std::unordered_map<std::string, std::vector<std::string>> spelling_dictionary;

                    nlohmann::json toJson() const;
                };
                std::optional<CustomSpellingConfig> custom_spelling_config;

                bool sentiment_analysis = false;

                bool audio_to_llm = false;

                /**
                 * Represents the configuration for audio-to-LLM processing.
                 */
                struct AudioToLLMConfig
                {
                    std::vector<std::string> prompts;
                    /**
                     * The model to use for the prompt execution, ex: "openai/gpt-5.4-nano".
                     */
                    std::optional<std::string> model;

                    nlohmann::json toJson() const;
                };
                std::optional<AudioToLLMConfig> audio_to_llm_config;

                bool pii_redaction = false;
                /**
                 * Represents the configuration for PII (Personally Identifiable Information) redaction.
                 */
                struct PiiRedactionConfig
                {
                    /**
                     * The entity types to redact, ex: "GDPR", "EMAIL_ADDRESS", "NAME", "PHONE_NUMBER".
                     */
                    std::vector<std::string> entity_types;

                    enum ProcessedTextType
                    {
                        MARKER,
                        MASK
                    };
                    /**
                     * The type of processed text to return (marker or mask).
                     */
                    std::optional<ProcessedTextType> processed_text_type;

                    nlohmann::json toJson() const;
                };
                std::optional<PiiRedactionConfig> pii_redaction_config;

                bool sentences = false;

                bool punctuation_enhanced = false;

                /**
                 * Custom metadata to attach to this transcription.
                 */
                std::optional<nlohmann::json> custom_metadata;

                /**
                 * Represents the configuration for language settings.
                 */
                struct LanguageConfig
                {
                    std::vector<std::string> languages;
                    bool code_switching = false;

                    nlohmann::json toJson() const;
                };
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
    }
}
