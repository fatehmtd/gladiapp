#include "gladiapp/gladiapp_ws_request.hpp"


nlohmann::json gladiapp::v2::request::InitializeSessionRequest::LanguageConfig::toJson() const {
    nlohmann::json json;
    json["languages"] = languages;
    json["code_switching"] = code_switching;
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::toJson() const
{
    nlohmann::json json;
    json["region"] = region;
    json["encoding"] = encoding;
    json["bitDepth"] = bitDepth;
    json["sampleRate"] = sampleRate;
    json["channels"] = channels;
    json["custom_metadata"] = custom_metadata;
    json["model"] = model;
    json["endpointing"] = endpointing;
    json["maximum_duration_without_endpointing"] = maximum_duration_without_endpointing;
    json["language_config"] = language_config.toJson();
    json["pre_processing"] = pre_processing.toJson();
    json["realtime_processing"] = realtime_processing.toJson();
    json["post_processing"] = post_processing.toJson();
    json["messages_config"] = messages_config.toJson();
    json["callback"] = callback;
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::PreProcessing::toJson() const
{
    nlohmann::json json;
    json["audio_enhancer"] = audio_enhancer;
    json["speech_threshold"] = speech_threshold;
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::Vocabulary::toJson() const
{
    nlohmann::json json;
    json["value"] = value;
    json["intensity"] = intensity;
    json["pronunciations"] = pronunciations;
    json["language"] = language;
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::CustomSpellingConfig::toJson() const 
{
    nlohmann::json json;
    json["spelling_dictionary"] = spelling_dictionary;
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::TranslationConfig::toJson() const {
    nlohmann::json json;
    json["model"] = model;
    json["target_languages"] = target_languages;
    if (match_original_utterances.has_value()) {
        json["match_original_utterances"] = match_original_utterances.value();
    }
    if (lipsync.has_value()) {
        json["lipsync"] = lipsync.value();
    }
    if (context_adaptation.has_value()) {
        json["context_adaptation"] = context_adaptation.value();
    }
    if (context.has_value()) {
        json["context"] = context.value();
    }
    if (informal.has_value()) {
        json["informal"] = informal.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::toJson() const
{
    nlohmann::json json;
    if (vocabulary.has_value()) {
        nlohmann::json vocab_array = nlohmann::json::array();
        for (const auto& vocab : vocabulary.value()) {
            vocab_array.push_back(vocab.toJson());
        }
        json["vocabulary"] = vocab_array;
    }
    if (default_intensity.has_value()) {
        json["default_intensity"] = default_intensity.value();
    }
    json["custom_spelling"] = custom_spelling;
    if (custom_spelling_config.has_value()) {
        json["custom_spelling_config"] = custom_spelling_config.value().toJson();
    }
    json["translation"] = translation;
    json["translation_config"] = translation_config.toJson();
    if (named_entity_recognition.has_value()) {
        json["named_entity_recognition"] = named_entity_recognition.value();
    }
    if (sentiment_analysis.has_value()) {
        json["sentiment_analysis"] = sentiment_analysis.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::PostProcessing::SummarizationConfig::toJson() const
{
    nlohmann::json json;
    for (const auto& type : types) {
        if (type == Type::GENERAL) {
            json["types"].push_back("general");
        } else if (type == Type::BULLET_POINTS) {
            json["types"].push_back("bullet_points");
        } else if (type == Type::CONCISE) {
            json["types"].push_back("concise");
        }
    }
    return json;
}