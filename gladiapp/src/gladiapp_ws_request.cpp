#include "gladiapp/gladiapp_ws_request.hpp"
#include "gladiapp_ws_request.hpp"

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::LanguageConfig::toJson() const
{
    nlohmann::json json;
    json["languages"] = languages;
    json["code_switching"] = code_switching;
    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::LanguageConfig gladiapp::v2::ws::request::InitializeSessionRequest::LanguageConfig::fromJson(const nlohmann::json &json)
{
    LanguageConfig config;
    config.languages = json.value("languages", std::vector<std::string>());
    config.code_switching = json.value("code_switching", false);
    return config;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::PreProcessing::toJson() const
{
    nlohmann::json json;
    json["audio_enhancer"] = audio_enhancer;
    json["speech_threshold"] = speech_threshold;
    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::PreProcessing gladiapp::v2::ws::request::InitializeSessionRequest::PreProcessing::fromJson(const nlohmann::json &json)
{
    PreProcessing config;
    config.audio_enhancer = json.value("audio_enhancer", false);
    config.speech_threshold = json.value("speech_threshold", 0.6);
    return config;
}

gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::Vocabulary gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::Vocabulary::fromJson(const nlohmann::json &json) {
    Vocabulary vocab;
    vocab.value = json.value("value", "");
    vocab.intensity = json.value("intensity", 0.5);
    if (json.contains("pronunciations") && json["pronunciations"].is_array())
    {
        for (const auto &pron : json["pronunciations"])
        {
            vocab.pronunciations.push_back(pron.get<std::string>());
        }
    }
    vocab.language = json.value("language", "en");
    return vocab;
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

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomSpellingConfig::toJson() const
{
    nlohmann::json json;
    json["spelling_dictionary"] = spelling_dictionary;
    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomSpellingConfig gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomSpellingConfig::fromJson(const nlohmann::json &json)
{
    CustomSpellingConfig config;
    config.spelling_dictionary = json.value("spelling_dictionary", std::unordered_map<std::string, std::vector<std::string>>());
    return config;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::TranslationConfig::toJson() const
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
    for (const auto &lang : target_languages)
    {
        json["target_languages"].push_back(lang);
    }

    json["match_original_utterances"] = match_original_utterances.has_value() ? match_original_utterances.value() : false;

    json["lipsync"] = lipsync.has_value() ? lipsync.value() : false;

    json["context_adaptation"] = context_adaptation.has_value() ? context_adaptation.value() : false;

    if (context.has_value())
    {
        json["context"] = context.value();
    }
    json["informal"] = informal.has_value() ? informal.value() : false;

    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::TranslationConfig gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::TranslationConfig::fromJson(const nlohmann::json &json)
{
    TranslationConfig config;
    if(json.contains("model")) {
        std::string modelStr = json.value("model", "base");
        if (modelStr == "base") {
            config.model = TranslationConfig::Model::BASE;
        } else if (modelStr == "enhanced") {
            config.model = TranslationConfig::Model::ENHANCED;
        }
    } else {
        config.model = TranslationConfig::Model::BASE; // Default value if "model" key is missing
    }
    config.target_languages = json.value("target_languages", std::vector<std::string>());
    config.match_original_utterances = json.value("match_original_utterances", true);
    config.lipsync = json.value("lipsync", true);
    config.context_adaptation = json.value("context_adaptation", false);
    if(json.contains("context")) {
        config.context = json.value("context", "");
    }
    config.informal = json.value("informal", false);
    return config;
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
    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::CustomVocabularyConfig::fromJson(const nlohmann::json &json)
{
    CustomVocabularyConfig config;
    if (json.contains("vocabulary") && json["vocabulary"].is_array())
    {
        auto vocabulary = std::vector<Vocabulary>();
        for (const auto &vocab : json["vocabulary"])
        {
            vocabulary.push_back(Vocabulary::fromJson(vocab));
        }
        config.vocabulary = vocabulary;
    }
    config.default_intensity = json.value("default_intensity", 0.5);
    return config;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing::SummarizationConfig::toJson() const
{
    nlohmann::json json;
    switch (type)
    {
    case Type::GENERAL:
        json["type"] = "general";
        break;
    case Type::BULLET_POINTS:
        json["type"] = "bullet_points";
        break;
    case Type::CONCISE:
        json["type"] = "concise";
        break;
    }
    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing::SummarizationConfig gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing::SummarizationConfig::fromJson(const nlohmann::json & json)
{
    SummarizationConfig config;
    if(json.contains("type")) {
        std::string typeStr = json.value("type", "general");
        if (typeStr == "general") {
            config.type = SummarizationConfig::Type::GENERAL;
        } else if (typeStr == "bullet_points") {
            config.type = SummarizationConfig::Type::BULLET_POINTS;
        } else if (typeStr == "concise") {
            config.type = SummarizationConfig::Type::CONCISE;
        }
    } else {
        config.type = SummarizationConfig::Type::GENERAL; // Default value if "type" key is missing
    }
    return config;
}

nlohmann::json gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::toJson() const
{
    nlohmann::json json;
    json["custom_vocabulary"] = custom_vocabulary;
    if (custom_vocabulary_config.has_value())
    {
        json["custom_vocabulary_config"] = custom_vocabulary_config->toJson();
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

gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing::fromJson(const nlohmann::json &json)
{
    RealtimeProcessing config;
    config.custom_vocabulary = json.value("custom_vocabulary", false);
    if (json.contains("custom_vocabulary_config")) {
        config.custom_vocabulary_config = CustomVocabularyConfig::fromJson(json["custom_vocabulary_config"]);
    }
    config.custom_spelling = json.value("custom_spelling", false);
    if (json.contains("custom_spelling_config")) {
        config.custom_spelling_config = CustomSpellingConfig::fromJson(json["custom_spelling_config"]);
    }
    config.translation = json.value("translation", false);
    if (json.contains("translation_config")) {
        config.translation_config = TranslationConfig::fromJson(json["translation_config"]);
    }
    config.named_entity_recognition = json.value("named_entity_recognition", false);
    config.sentiment_analysis = json.value("sentiment_analysis", false);
    return config;
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
        json["summarization_config"] = summarization_config->toJson();
    }
    if (chapterization.has_value())
    {
        json["chapterization"] = chapterization.value();
    }
    return json;
}

gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing gladiapp::v2::ws::request::InitializeSessionRequest::PostProcessing::fromJson(const nlohmann::json &json)
{
    PostProcessing config;
    config.summarization = json.value("summarization", false);
    if (json.contains("summarization_config")) {
        config.summarization_config = SummarizationConfig::fromJson(json["summarization_config"]);
    }
    config.chapterization = json.value("chapterization", false);
    return config;
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
    if (receive_acknowledgments.has_value())
    {
        json["receive_acknowledgments"] = receive_acknowledgments.value();
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

gladiapp::v2::ws::request::InitializeSessionRequest::MessagesConfig gladiapp::v2::ws::request::InitializeSessionRequest::MessagesConfig::fromJson(const nlohmann::json &json)
{
    MessagesConfig config;
    config.receive_partial_transcripts = json.value("receive_partial_transcripts", false);
    config.receive_final_transcripts = json.value("receive_final_transcripts", true);
    config.receive_speech_events = json.value("receive_speech_events", false);
    config.receive_pre_processing_events = json.value("receive_pre_processing_events", false);
    config.receive_realtime_processing_events = json.value("receive_realtime_processing_events", false);
    config.receive_post_processing_events = json.value("receive_post_processing_events", false);
    config.receive_acknowledgments = json.value("receive_acknowledgments", false);
    config.receive_errors = json.value("receive_errors", false);
    config.receive_lifecycle_events = json.value("receive_lifecycle_events", false);
    return config;
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

    if (receive_acknowledgments.has_value())
    {
        json["receive_acknowledgements"] = receive_acknowledgments.value();
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

gladiapp::v2::ws::request::InitializeSessionRequest::CallbackConfig gladiapp::v2::ws::request::InitializeSessionRequest::CallbackConfig::fromJson(const nlohmann::json &json)
{
    CallbackConfig config;
    if(json.contains("url")) {
        config.url = json.value("url", "");
    }
    config.receive_partial_transcripts = json.value("receive_partial_transcripts", false);
    config.receive_final_transcripts = json.value("receive_final_transcripts", true);
    config.receive_speech_events = json.value("receive_speech_events", false);
    config.receive_pre_processing_events = json.value("receive_pre_processing_events", false);
    config.receive_realtime_processing_events = json.value("receive_realtime_processing_events", false);
    config.receive_post_processing_events = json.value("receive_post_processing_events", false);
    config.receive_acknowledgments = json.value("receive_acknowledgments", false);
    config.receive_errors = json.value("receive_errors", false);
    config.receive_lifecycle_events = json.value("receive_lifecycle_events", false);
    return config;
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

gladiapp::v2::ws::request::InitializeSessionRequest gladiapp::v2::ws::request::InitializeSessionRequest::fromJson(const nlohmann::json &json)
{
    InitializeSessionRequest request;
    std::string encodingStr = json.value("encoding", "wav/pcm");
    if (encodingStr == "wav/pcm") {
        request.encoding = Encoding::WAV_PCM;
    } else if (encodingStr == "wav/alaw") {
        request.encoding = Encoding::WAV_ALAW;
    } else if (encodingStr == "wav/ulaw") {
        request.encoding = Encoding::WAV_ULAW;
    }
    int bitDepthValue = json.value("bit_depth", 16);
    switch (bitDepthValue) {
        case 8:
            request.bit_depth = BitDepth::BIT_DEPTH_8;
            break;
        case 16:
            request.bit_depth = BitDepth::BIT_DEPTH_16;
            break;
        case 24:
            request.bit_depth = BitDepth::BIT_DEPTH_24;
            break;
        case 32:
            request.bit_depth = BitDepth::BIT_DEPTH_32;
            break;
        default:
            request.bit_depth = BitDepth::BIT_DEPTH_16; // Default value
            break;
    }
    int sampleRateValue = json.value("sample_rate", 16000);
    switch (sampleRateValue) {
        case 8000:
            request.sample_rate = SampleRate::SAMPLE_RATE_8000;
            break;
        case 16000:
            request.sample_rate = SampleRate::SAMPLE_RATE_16000;
            break;
        case 32000:
            request.sample_rate = SampleRate::SAMPLE_RATE_32000;
            break;
        case 44100:
            request.sample_rate = SampleRate::SAMPLE_RATE_44100;
            break;
        case 48000:
            request.sample_rate = SampleRate::SAMPLE_RATE_48000;
            break;
        default:
            request.sample_rate = SampleRate::SAMPLE_RATE_16000; // Default value
            break;
    }
    request.channels = json.value("channels", 1);
    if(json.contains("custom_metadata")) {
        request.custom_metadata = json.value("custom_metadata", "");
    }
    std::string modelStr = json.value("model", "solaria-1");
    if (modelStr == "solaria-1") {
        request.model = Model::SOLARIA_1;
    } else {
        request.model = Model::SOLARIA_1; // Default value if unknown
    }
    request.endpointing = json.value("endpointing", 0.05);
    request.maximum_duration_without_endpointing = json.value("maximum_duration_without_endpointing", 5);
    if (json.contains("language_config")) {
        request.language_config = LanguageConfig::fromJson(json["language_config"]);
    }
    if (json.contains("pre_processing")) {
        request.pre_processing = PreProcessing::fromJson(json["pre_processing"]);
    }
    if (json.contains("realtime_processing")) {
        request.realtime_processing = RealtimeProcessing::fromJson(json["realtime_processing"]);
    }
    if (json.contains("post_processing")) {
        request.post_processing = PostProcessing::fromJson(json["post_processing"]);
    }
    if (json.contains("messages_config")) {
        request.messages_config = MessagesConfig::fromJson(json["messages_config"]);
    }
    request.callback = json.value("callback", false);
    if (json.contains("callback_config")) {
        request.callback_config = CallbackConfig::fromJson(json["callback_config"]);
    }    
    return request;
}
