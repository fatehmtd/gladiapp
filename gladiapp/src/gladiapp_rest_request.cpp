#include "gladiapp/gladiapp_rest.hpp"
#include <algorithm>
#include <unordered_map>

namespace gladiapp
{
    namespace v2
    {
        namespace request
        {
            // DiarizationConfig implementations
            nlohmann::json DiarizationConfig::toJson() const
            {
                nlohmann::json j;
                j["number_of_speakers"] = number_of_speakers;
                j["min_speakers"] = min_speakers;
                j["max_speakers"] = max_speakers;
                if (enhanced.has_value())
                {
                    j["enhanced"] = enhanced.value();
                }
                return j;
            }

            // TranslationConfig implementations
            nlohmann::json TranslationConfig::toJson() const
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
            std::string SubtitlesConfig::toString() const
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
                j["maximum_characters_per_row"] = maximum_characters_per_row;
                j["maximum_rows_per_caption"] = maximum_rows_per_caption;
                if (style.has_value())
                {
                    j["style"] = style.value() == Style::DEFAULT ? "default" : "compliance";
                }
                return j.dump();
            }

            // CallbackConfig implementations
            std::string CallbackConfig::toJson() const
            {
                nlohmann::json j;
                j["url"] = url;
                j["method"] = (method == HttpMethod::POST) ? "POST" : "PUT";
                return j.dump();
            }

            // SummarizationConfig implementations
            std::string SummarizationConfig::toJson() const
            {
                nlohmann::json j;
                std::vector<std::string> typeStrings;
                for (const auto &type : types)
                {
                    switch (type)
                    {
                    case GENERAL:
                        typeStrings.push_back("general");
                        break;
                    case BULLET_POINTS:
                        typeStrings.push_back("bullet_points");
                        break;
                    case CONCISE:
                        typeStrings.push_back("concise");
                        break;
                    }
                }
                j["summarization_config"] = typeStrings;
                return j.dump();
            }

            // CustomSpellingConfig implementations
            nlohmann::json CustomSpellingConfig::toJson() const
            {
                nlohmann::json j;
                j["spelling_dictionary"] = spelling_dictionary;
                return j;
            }

            // StructuredDataExtractionConfig implementations
            nlohmann::json StructuredDataExtractionConfig::toJson() const
            {
                nlohmann::json j;
                j["structured_data_extraction_config"] = classes;
                return j;
            }

            // AudioToLLMConfig implementations
            nlohmann::json AudioToLLMConfig::toJson() const
            {
                nlohmann::json j;
                j["prompts"] = prompts;
                return j;
            }

            // LanguageConfig implementations
            nlohmann::json LanguageConfig::toJson() const
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
                j["custom_vocabulary"] = customVocabulary;
                if (customVocabulary && !custom_vocabulary_config.empty())
                {
                    nlohmann::json vocab_array = nlohmann::json::array();
                    for (const auto &vocab : custom_vocabulary_config)
                    {
                        nlohmann::json vocab_obj;
                        vocab_obj["value"] = vocab.value;
                        vocab_obj["pronunciations"] = vocab.pronunciations;
                        if (vocab.intensity.has_value())
                        {
                            vocab_obj["intensity"] = vocab.intensity.value();
                        }
                        if (vocab.language.has_value())
                        {
                            vocab_obj["language"] = vocab.language.value();
                        }
                        vocab_array.push_back(vocab_obj);
                    }
                    j["custom_vocabulary_config"] = vocab_array;
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
                    j["summarization_config"] = nlohmann::json::parse(summarization_config->toJson())["summarization_config"];
                }

                // Boolean flags
                j["moderation"] = moderation;
                j["named_entity_recognition"] = named_entity_recognition;
                j["chapterization"] = chapterization;
                j["name_consistency"] = name_consistency;
                j["sentiment_analysis"] = sentiment_analysis;
                j["sentences"] = sentences;
                j["display_mode"] = display_mode;
                j["punctuation_enhanced"] = punctuation_enhanced;

                // Custom spelling
                j["custom_spelling"] = custom_spelling;
                if (custom_spelling && custom_spelling_config.has_value())
                {
                    j["custom_spelling_config"] = custom_spelling_config->toJson()["spelling_dictionary"];
                }

                // Structured data extraction
                j["structured_data_extraction"] = structured_data_extraction;
                if (structured_data_extraction && structured_data_extraction_config.has_value())
                {
                    j["structured_data_extraction_config"] = structured_data_extraction_config->toJson()["structured_data_extraction_config"];
                }

                // Audio to LLM
                j["audio_to_llm"] = audio_to_llm;
                if (audio_to_llm && audio_to_llm_config.has_value())
                {
                    j["audio_to_llm_config"] = audio_to_llm_config->toJson()["prompts"];
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