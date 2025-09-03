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
            namespace request
            {
                /**
                 * Contains the parameters for initializing a WebSocket session.
                 */
                struct GLADIAPP_EXPORT InitializeSessionRequest
                {
                    enum Region
                    {
                        US_WEST,
                        EU_WEST
                    };
                    Region region;

                    enum Encoding
                    {
                        WAV_PCM,
                        WAV_ALAW,
                        WAV_ULAW
                    };
                    Encoding encoding;

                    enum BitDepth
                    {
                        BIT_DEPTH_8,
                        BIT_DEPTH_16,
                        BIT_DEPTH_24,
                        BIT_DEPTH_32
                    };
                    BitDepth bit_depth;

                    enum SampleRate
                    {
                        SAMPLE_RATE_8000,
                        SAMPLE_RATE_16000,
                        SAMPLE_RATE_32000,
                        SAMPLE_RATE_44100,
                        SAMPLE_RATE_48000
                    };
                    SampleRate sample_rate;

                    /**
                     * Number of audio channels
                     */
                    int channels = 1;

                    std::optional<std::string> custom_metadata;

                    enum Model
                    {
                        SOLARIA_1
                    };
                    Model model = Model::SOLARIA_1;

                    /**
                     * Endpointing timeout (in seconds)
                     */
                    double endpointing = 0.05;

                    /**
                     * Maximum duration without endpointing (in seconds)
                     */
                    int maximum_duration_without_endpointing = 5;

                    /**
                     * Language configuration
                     */
                    struct LanguageConfig
                    {
                        std::vector<std::string> languages;
                        /**
                         * Enables or disables code switching (language switching).
                         */
                        bool code_switching = false;

                        nlohmann::json toJson() const;
                    };
                    std::optional<LanguageConfig> language_config;

                    /**
                     * Pre-processing configuration
                     */
                    struct PreProcessing
                    {
                        bool audio_enhancer = false;
                        /**
                         * VAD threshold, between 0 (Permissive) and 1 (Strict)
                         */
                        double speech_threshold = 0.6;

                        nlohmann::json toJson() const;
                    };
                    std::optional<PreProcessing> pre_processing;

                    /**
                     * Real-time processing configuration
                     */
                    struct RealtimeProcessing
                    {
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
                                double intensity;
                                std::vector<std::string> pronunciations;
                                std::string language;

                                nlohmann::json toJson() const;
                            };
                            std::optional<std::vector<Vocabulary>> vocabulary;
                            std::optional<double> default_intensity = 0.5;

                            bool custom_spelling = false;
                            /**
                             * Represents the configuration for custom spelling.
                             */
                            struct CustomSpellingConfig
                            {
                                /**
                                 *  Mapping of words to their custom spellings.
                                 *  ex: {"Night's Watch": ["Nights Watch", "Nightz Watch"]}
                                 */
                                std::unordered_map<std::string, std::vector<std::string>> spelling_dictionary;

                                nlohmann::json toJson() const;
                            };
                            std::optional<CustomSpellingConfig> custom_spelling_config;

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
                                /**
                                 * Represents the target languages for translation.
                                 */
                                std::vector<std::string> target_languages;
                                /**
                                 * Enables or disables matching original utterances.
                                 */
                                std::optional<bool> match_original_utterances = true;
                                /**
                                 * Enables or disables lip sync for translated audio.
                                 */
                                std::optional<bool> lipsync = true;
                                /**
                                 * Enables or disables context adaptation for translated audio.
                                 */
                                std::optional<bool> context_adaptation = false;
                                /**
                                 * Optional context to improve translation accuracy.
                                 */
                                std::optional<std::string> context;
                                /**
                                 * Enables or disables informal tone for translated audio.
                                 */
                                std::optional<bool> informal = false;

                                nlohmann::json toJson() const;
                            };
                            TranslationConfig translation_config;

                            /**
                             * Enables or disables named entity recognition.
                             */
                            std::optional<bool> named_entity_recognition = false;

                            /**
                             * Enables or disables sentiment analysis.
                             */
                            std::optional<bool> sentiment_analysis = false;

                            nlohmann::json toJson() const;
                        };
                        std::optional<CustomVocabularyConfig> custom_vocabulary_config;

                        nlohmann::json toJson() const;
                    };
                    std::optional<RealtimeProcessing> realtime_processing;

                    /**
                     * Represents the configuration for post-processing.
                     */
                    struct PostProcessing
                    {
                        std::optional<bool> summarization = false;
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

                            nlohmann::json toJson() const;
                        };
                        std::optional<SummarizationConfig> summarization_config;

                        std::optional<bool> chapterization = false;

                        nlohmann::json toJson() const;
                    };
                    std::optional<PostProcessing> post_processing;

                    /**
                     * Represents the configuration for message handling.
                     */
                    struct MessagesConfig
                    {
                        std::optional<bool> receive_partial_transcripts = false;
                        std::optional<bool> receive_final_transcripts = true;
                        std::optional<bool> receive_speech_events = true;
                        std::optional<bool> receive_pre_processing_events = true;
                        std::optional<bool> receive_realtime_processing_events = true;
                        std::optional<bool> receive_post_processing_events = true;
                        std::optional<bool> receive_acknowledgements = true;
                        std::optional<bool> receive_errors = true;
                        std::optional<bool> receive_lifecycle_events = true;

                        nlohmann::json toJson() const;
                    };
                    std::optional<MessagesConfig> messages_config;

                    std::optional<bool> callback = false;
                    /**
                     * Represents the configuration for callbacks.
                     */
                    struct CallbackConfig
                    {
                        std::optional<std::string> url;
                        std::optional<bool> receive_partial_transcripts = false;
                        std::optional<bool> receive_final_transcripts = true;
                        std::optional<bool> receive_speech_events = false;
                        std::optional<bool> receive_pre_processing_events = true;
                        std::optional<bool> receive_realtime_processing_events = true;
                        std::optional<bool> receive_post_processing_events = true;
                        std::optional<bool> receive_acknowledgements = false;
                        std::optional<bool> receive_errors = false;
                        std::optional<bool> receive_lifecycle_events = true;

                        nlohmann::json toJson() const;
                    };
                    std::optional<CallbackConfig> callback_config;

                    nlohmann::json toJson() const;
                };
            }
        }
    }
}