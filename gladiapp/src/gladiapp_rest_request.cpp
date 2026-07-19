#include "gladiapp/gladiapp_rest_request.hpp"
#include <algorithm>
#include <unordered_map>

namespace gladiapp
{
    namespace v2
    {
        namespace request
        {
            // DiarizationConfig implementations
            nlohmann::json TranscriptionRequest::DiarizationConfig::toJson() const
            {
                nlohmann::json j;
                if (number_of_speakers.has_value())
                {
                    j["number_of_speakers"] = number_of_speakers.value();
                }
                if (min_speakers.has_value())
                {
                    j["min_speakers"] = min_speakers.value();
                }
                if (max_speakers.has_value())
                {
                    j["max_speakers"] = max_speakers.value();
                }
                return j;
            }

            // TranslationConfig implementations
            nlohmann::json TranscriptionRequest::TranslationConfig::toJson() const
            {
                nlohmann::json j;
                j["model"] = model == Model::BASE ? "base" : "enhanced";
                j["target_languages"] = target_languages;
                if (match_original_utterances.has_value())
                {
                    j["match_original_utterances"] = match_original_utterances.value();
                }
                if (lipsync.has_value())
                {
                    j["lipsync"] = lipsync.value();
                }
                if (context_adaptation.has_value())
                {
                    j["context_adaptation"] = context_adaptation.value();
                }
                if (context.has_value())
                {
                    j["context"] = context.value();
                }
                if (informal.has_value())
                {
                    j["informal"] = informal.value();
                }
                return j;
            }

            // SubtitlesConfig implementations
            std::string TranscriptionRequest::SubtitlesConfig::toString() const
            {
                nlohmann::json j;
                std::vector<std::string> formats;
                std::transform(this->formats.begin(), this->formats.end(), std::back_inserter(formats),
                               [](Format format) -> std::string
                               {
                                   switch (format)
                                   {
                                   case Format::SRT:
                                       return "srt";
                                   case Format::VTT:
                                       return "vtt";
                                   default:
                                       return "";
                                   }
                               });
                j["formats"] = formats;
                if (minimum_duration.has_value())
                {
                    j["minimum_duration"] = minimum_duration.value();
                }
                if (maximum_duration.has_value())
                {
                    j["maximum_duration"] = maximum_duration.value();
                }
                if (maximum_characters_per_row.has_value())
                {
                    j["maximum_characters_per_row"] = maximum_characters_per_row.value();
                }
                if (maximum_rows_per_caption.has_value())
                {
                    j["maximum_rows_per_caption"] = maximum_rows_per_caption.value();
                }
                if (style.has_value())
                {
                    j["style"] = style.value() == Style::DEFAULT ? "default" : "compliance";
                }
                return j.dump();
            }

            // CallbackConfig implementations
            std::string TranscriptionRequest::CallbackConfig::toJson() const
            {
                nlohmann::json j;
                j["url"] = url;
                j["method"] = (method == HttpMethod::POST) ? "POST" : "PUT";
                return j.dump();
            }

            // SummarizationConfig implementations
            nlohmann::json TranscriptionRequest::SummarizationConfig::toJson() const
            {
                nlohmann::json j;
                switch (type)
                {
                case GENERAL:
                    j["type"] = "general";
                    break;
                case BULLET_POINTS:
                    j["type"] = "bullet_points";
                    break;
                case CONCISE:
                    j["type"] = "concise";
                    break;
                }
                return j;
            }

            // CustomVocabularyConfig implementations
            nlohmann::json TranscriptionRequest::CustomVocabularyConfig::Vocabulary::toJson() const
            {
                nlohmann::json j;
                j["value"] = value;
                if (!pronunciations.empty())
                {
                    j["pronunciations"] = pronunciations;
                }
                if (intensity.has_value())
                {
                    j["intensity"] = intensity.value();
                }
                if (language.has_value())
                {
                    j["language"] = language.value();
                }
                return j;
            }

            nlohmann::json TranscriptionRequest::CustomVocabularyConfig::toJson() const
            {
                nlohmann::json j;
                nlohmann::json vocabularyArray = nlohmann::json::array();
                for (const auto &entry : vocabulary)
                {
                    vocabularyArray.push_back(entry.toJson());
                }
                j["vocabulary"] = vocabularyArray;
                if (default_intensity.has_value())
                {
                    j["default_intensity"] = default_intensity.value();
                }
                return j;
            }

            // CustomSpellingConfig implementations
            nlohmann::json TranscriptionRequest::CustomSpellingConfig::toJson() const
            {
                nlohmann::json j;
                j["spelling_dictionary"] = spelling_dictionary;
                return j;
            }

            // AudioToLLMConfig implementations
            nlohmann::json TranscriptionRequest::AudioToLLMConfig::toJson() const
            {
                nlohmann::json j;
                j["prompts"] = prompts;
                if (model.has_value())
                {
                    j["model"] = model.value();
                }
                return j;
            }

            // PiiRedactionConfig implementations
            nlohmann::json TranscriptionRequest::PiiRedactionConfig::toJson() const
            {
                nlohmann::json j;
                j["entity_types"] = entity_types;
                if (processed_text_type.has_value())
                {
                    j["processed_text_type"] = processed_text_type.value() == ProcessedTextType::MARKER ? "MARKER" : "MASK";
                }
                return j;
            }

            // LanguageConfig implementations
            nlohmann::json TranscriptionRequest::LanguageConfig::toJson() const
            {
                nlohmann::json j;
                j["languages"] = languages;
                j["code_switching"] = code_switching;
                return j;
            }

            // TranscriptionRequest implementations
            nlohmann::json TranscriptionRequest::toJson() const
            {
                nlohmann::json j;

                j["audio_url"] = audio_url;

                // Custom vocabulary
                j["custom_vocabulary"] = custom_vocabulary;
                if (custom_vocabulary && custom_vocabulary_config.has_value())
                {
                    j["custom_vocabulary_config"] = custom_vocabulary_config->toJson();
                }

                // Callback
                j["callback"] = callback;
                if (callback && callback_config.has_value())
                {
                    j["callback_config"] = nlohmann::json::parse(callback_config->toJson());
                }

                // Subtitles
                j["subtitles"] = subtitles;
                if (subtitles && subtitles_config.has_value())
                {
                    j["subtitles_config"] = nlohmann::json::parse(subtitles_config->toString());
                }

                // Diarization
                j["diarization"] = diarization;
                if (diarization && diarization_config.has_value())
                {
                    j["diarization_config"] = diarization_config->toJson();
                }

                // Translation
                j["translation"] = translation;
                if (translation)
                {
                    j["translation_config"] = translation_config.toJson();
                }

                // Summarization
                j["summarization"] = summarization;
                if (summarization && summarization_config.has_value())
                {
                    j["summarization_config"] = summarization_config->toJson();
                }

                // Boolean flags
                j["named_entity_recognition"] = named_entity_recognition;
                j["sentiment_analysis"] = sentiment_analysis;
                j["sentences"] = sentences;
                j["punctuation_enhanced"] = punctuation_enhanced;

                // Custom spelling
                j["custom_spelling"] = custom_spelling;
                if (custom_spelling && custom_spelling_config.has_value())
                {
                    j["custom_spelling_config"] = custom_spelling_config->toJson();
                }

                // Audio to LLM
                j["audio_to_llm"] = audio_to_llm;
                if (audio_to_llm && audio_to_llm_config.has_value())
                {
                    j["audio_to_llm_config"] = audio_to_llm_config->toJson();
                }

                // PII redaction
                j["pii_redaction"] = pii_redaction;
                if (pii_redaction && pii_redaction_config.has_value())
                {
                    j["pii_redaction_config"] = pii_redaction_config->toJson();
                }

                // Custom metadata
                if (custom_metadata.has_value())
                {
                    j["custom_metadata"] = custom_metadata.value();
                }

                // Language config
                if (language_config.has_value())
                {
                    j["language_config"] = language_config.value().toJson();
                }

                return j;
            }
        }
    }
}

std::string gladiapp::v2::request::ListResultsQuery::toString() const
{
    nlohmann::json json;
    json["offset"] = offset;
    json["limit"] = limit;
    for (const auto &s : status)
    {
        switch (s)
        {
        case Status::QUEUED:
            json["status"].push_back("queued");
            break;
        case Status::DONE:
            json["status"].push_back("done");
            break;
        case Status::ERROR:
            json["status"].push_back("error");
            break;
        case Status::PROCESSING:
            json["status"].push_back("processing");
            break;
        }
    }
    return json.dump();
}
