#include <string>
#include <vector>
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

                bool customVocabulary = false;
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
                std::vector<CustomVocabularyConfig> custom_vocabulary_config;

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
                    std::vector<Type> types;

                    std::string toJson() const;
                };                
                std::optional<SummarizationConfig> summarization_config;

                bool moderation = false;

                bool named_entity_recognition = false;

                bool chapterization = false;

                bool name_consistency = false;

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

                bool structured_data_extraction = false;
                /**
                 * Represents the configuration for structured data extraction.
                 */
                struct StructuredDataExtractionConfig
                {
                    std::vector<std::string> classes;

                    nlohmann::json toJson() const;
                };
                std::optional<StructuredDataExtractionConfig> structured_data_extraction_config;

                bool sentiment_analysis = false;

                bool audio_to_llm = false;

                /**
                 * Represents the configuration for audio-to-LLM processing.
                 */
                struct AudioToLLMConfig
                {
                    std::vector<std::string> prompts;

                    nlohmann::json toJson() const;
                };
                std::optional<AudioToLLMConfig> audio_to_llm_config;

                bool sentences = false;

                bool display_mode = false;

                bool punctuation_enhanced = false;

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