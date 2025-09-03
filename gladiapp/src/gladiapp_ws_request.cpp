#include "gladiapp/gladiapp_ws_request.hpp"

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::LanguageConfig::toJson() const
{
    nlohmann::json json;
    json["languages"] = languages;
    json["code_switching"] = code_switching;
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::PreProcessing::toJson() const
{
    nlohmann::json json;
    json["audio_enhancer"] = audio_enhancer;
    json["speech_threshold"] = speech_threshold;
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::Vocabulary::toJson() const
{
    nlohmann::json json;
    json["value"] = value;
    json["intensity"] = intensity;
    json["pronunciations"] = pronunciations;
    json["language"] = language;
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::CustomSpellingConfig::toJson() const
{
    nlohmann::json json;
    json["spelling_dictionary"] = spelling_dictionary;
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::TranslationConfig::toJson() const
{
    nlohmann::json json;
    switch (model)
    {
    case Model::BASE:
        json["model"] = "base";
        break;
    case Model::ENHANCED:
        json["model"] = "enhanced";
        break;
    }
    json["target_languages"] = target_languages;
    if (match_original_utterances.has_value())
    {
        json["match_original_utterances"] = match_original_utterances.value();
    }
    if (lipsync.has_value())
    {
        json["lipsync"] = lipsync.value();
    }
    if (context_adaptation.has_value())
    {
        json["context_adaptation"] = context_adaptation.value();
    }
    if (context.has_value())
    {
        json["context"] = context.value();
    }
    if (informal.has_value())
    {
        json["informal"] = informal.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::toJson() const
{
    nlohmann::json json;
    if (vocabulary.has_value())
    {
        nlohmann::json vocab_array = nlohmann::json::array();
        for (const auto &vocab : vocabulary.value())
        {
            vocab_array.push_back(vocab.toJson());
        }
        json["vocabulary"] = vocab_array;
    }
    if (default_intensity.has_value())
    {
        json["default_intensity"] = default_intensity.value();
    }
    json["custom_spelling"] = custom_spelling;
    if (custom_spelling_config.has_value())
    {
        json["custom_spelling_config"] = custom_spelling_config.value().toJson();
    }
    json["translation"] = translation;
    json["translation_config"] = translation_config.toJson();
    if (named_entity_recognition.has_value())
    {
        json["named_entity_recognition"] = named_entity_recognition.value();
    }
    if (sentiment_analysis.has_value())
    {
        json["sentiment_analysis"] = sentiment_analysis.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing::SummarizationConfig::toJson() const
{
    nlohmann::json json;
    for (const auto &type : types)
    {
        if (type == Type::GENERAL)
        {
            json["types"].push_back("general");
        }
        else if (type == Type::BULLET_POINTS)
        {
            json["types"].push_back("bullet_points");
        }
        else if (type == Type::CONCISE)
        {
            json["types"].push_back("concise");
        }
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::toJson() const
{
    nlohmann::json json;
    json["custom_vocabulary"] = custom_vocabulary;
    if (custom_vocabulary_config.has_value())
    {
        json["custom_vocabulary_config"] = custom_vocabulary_config->toJson();
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing::toJson() const
{
    nlohmann::json json;
    if (summarization.has_value())
    {
        json["summarization"] = summarization.value();
    }
    if (summarization_config.has_value())
    {
        json["summarization_config"] = summarization_config.value().toJson();
    }
    if (chapterization.has_value())
    {
        json["chapterization"] = chapterization.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::MessagesConfig::toJson() const
{
    nlohmann::json json;
    if (receive_partial_transcripts.has_value())
    {
        json["receive_partial_transcripts"] = receive_partial_transcripts.value();
    }
    if (receive_final_transcripts.has_value())
    {
        json["receive_final_transcripts"] = receive_final_transcripts.value();
    }
    if (receive_speech_events.has_value())
    {
        json["receive_speech_events"] = receive_speech_events.value();
    }
    if (receive_pre_processing_events.has_value())
    {
        json["receive_pre_processing_events"] = receive_pre_processing_events.value();
    }
    if (receive_realtime_processing_events.has_value())
    {
        json["receive_realtime_processing_events"] = receive_realtime_processing_events.value();
    }
    if (receive_post_processing_events.has_value())
    {
        json["receive_post_processing_events"] = receive_post_processing_events.value();
    }
    if (receive_acknowledgements.has_value())
    {
        json["receive_acknowledgements"] = receive_acknowledgements.value();
    }
    if (receive_errors.has_value())
    {
        json["receive_errors"] = receive_errors.value();
    }
    if (receive_lifecycle_events.has_value())
    {
        json["receive_lifecycle_events"] = receive_lifecycle_events.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::CallbackConfig::toJson() const
{
    nlohmann::json json;
    if (url.has_value())
    {
        json["url"] = url.value();
    }

    if (receive_partial_transcripts.has_value())
    {
        json["receive_partial_transcripts"] = receive_partial_transcripts.value();
    }

    if (receive_final_transcripts.has_value())
    {
        json["receive_final_transcripts"] = receive_final_transcripts.value();
    }

    if (receive_speech_events.has_value())
    {
        json["receive_speech_events"] = receive_speech_events.value();
    }

    if (receive_pre_processing_events.has_value())
    {
        json["receive_pre_processing_events"] = receive_pre_processing_events.value();
    }

    if (receive_realtime_processing_events.has_value())
    {
        json["receive_realtime_processing_events"] = receive_realtime_processing_events.value();
    }

    if (receive_post_processing_events.has_value())
    {
        json["receive_post_processing_events"] = receive_post_processing_events.value();
    }

    if (receive_acknowledgements.has_value())
    {
        json["receive_acknowledgements"] = receive_acknowledgements.value();
    }

    if (receive_errors.has_value())
    {
        json["receive_errors"] = receive_errors.value();
    }

    if (receive_lifecycle_events.has_value())
    {
        json["receive_lifecycle_events"] = receive_lifecycle_events.value();
    }
    return json;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::toJson() const
{
    nlohmann::json json;

    switch (encoding)
    {
    case Encoding::WAV_PCM:
        json["encoding"] = "wav/pcm";
        break;
    case Encoding::WAV_ALAW:
        json["encoding"] = "wav/alaw";
        break;
    case Encoding::WAV_ULAW:
        json["encoding"] = "wav/ulaw";
        break;
    }
    switch (bit_depth)
    {
    case BitDepth::BIT_DEPTH_8:
        json["bit_depth"] = 8;
        break;
    case BitDepth::BIT_DEPTH_16:
        json["bit_depth"] = 16;
        break;
    case BitDepth::BIT_DEPTH_24:
        json["bit_depth"] = 24;
        break;
    case BitDepth::BIT_DEPTH_32:
        json["bit_depth"] = 32;
        break;
    }
    switch (sample_rate)
    {
    case SampleRate::SAMPLE_RATE_8000:
        json["sample_rate"] = 8000;
        break;
    case SampleRate::SAMPLE_RATE_16000:
        json["sample_rate"] = 16000;
        break;
    case SampleRate::SAMPLE_RATE_32000:
        json["sample_rate"] = 32000;
        break;
    case SampleRate::SAMPLE_RATE_44100:
        json["sample_rate"] = 44100;
        break;
    case SampleRate::SAMPLE_RATE_48000:
        json["sample_rate"] = 48000;
        break;
    }
    json["channels"] = channels;
    if (custom_metadata.has_value())
    {
        json["custom_metadata"] = custom_metadata.value();
    }
    switch (model)
    {
    case Model::SOLARIA_1:
        json["model"] = "solaria-1";
        break;
    default:
        json["model"] = "unknown";
        break;
    }
    json["endpointing"] = endpointing;
    json["maximum_duration_without_endpointing"] = maximum_duration_without_endpointing;
    if (language_config.has_value())
    {
        json["language_config"] = language_config->toJson();
    }
    if (pre_processing.has_value())
    {
        json["pre_processing"] = pre_processing->toJson();
    }
    if (realtime_processing.has_value())
    {
        json["realtime_processing"] = realtime_processing->toJson();
    }
    if (post_processing.has_value())
    {
        json["post_processing"] = post_processing->toJson();
    }
    if (messages_config.has_value())
    {
        json["messages_config"] = messages_config->toJson();
    }
    json["callback"] = callback;
    if (callback_config.has_value())
    {
        json["callback_config"] = callback_config->toJson();
    }
    return json;
}