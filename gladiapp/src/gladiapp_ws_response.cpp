#include "gladiapp_ws_response.hpp"

using namespace gladiapp::v2::ws::response;

InitializeSessionResponse gladiapp::v2::ws::response::InitializeSessionResponse::fromJson(const nlohmann::json &json)
{
    InitializeSessionResponse response;
    response.id = json.at("id").get<std::string>();
    response.url = json.at("url").get<std::string>();
    return response;
}

SpeechEvent gladiapp::v2::ws::response::SpeechEvent::fromJson(const nlohmann::json &json)
{
    SpeechEvent event;
    event.session_id = json.at("session_id").get<std::string>();
    event.created_at = json.at("created_at").get<std::string>();
    event.type = json.at("type").get<std::string>();
    event.data.time = json.at("data").at("time").get<double>();
    event.data.channel = json.at("data").at("channel").get<int>();
    return event;
}

Error gladiapp::v2::ws::response::Error::fromJson(const nlohmann::json &json)
{
    Error error;
    error.message = json.at("message").get<std::string>();
    return error;
}

Utterance::Word gladiapp::v2::ws::response::Utterance::Word::fromJson(const nlohmann::json &json)
{
    Word word;
    word.word = json.at("word").get<std::string>();
    word.start = json.at("start").get<double>();
    word.end = json.at("end").get<double>();
    word.confidence = json.at("confidence").get<double>();
    return word;
}

Utterance gladiapp::v2::ws::response::Utterance::fromJson(const nlohmann::json &json)
{
    Utterance utterance;
    utterance.language = json.at("language").get<std::string>();
    utterance.start = json.at("start").get<double>();
    utterance.end = json.at("end").get<double>();
    utterance.confidence = json.at("confidence").get<double>();
    utterance.channel = json.at("channel").get<int>();
    for (const auto &wordJson : json.at("words"))
    {
        utterance.words.push_back(Utterance::Word::fromJson(wordJson));
    }
    utterance.text = json.at("text").get<std::string>();
    if (json.contains("speaker"))
    {
        utterance.speaker = json.at("speaker").get<int>();
    }
    return utterance;
}

Transcript gladiapp::v2::ws::response::Transcript::fromJson(const nlohmann::json &json)
{
    Transcript transcript;
    transcript.session_id = json.at("session_id").get<std::string>();
    transcript.created_at = json.at("created_at").get<std::string>();
    transcript.type = json.at("type").get<std::string>();
    transcript.data.id = json.at("data").at("id").get<std::string>();
    transcript.data.is_final = json.at("data").at("is_final").get<bool>();
    transcript.data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
    return transcript;
}

Translation gladiapp::v2::ws::response::Translation::fromJson(const nlohmann::json &json)
{
    Translation translation;
    translation.session_id = json.at("session_id").get<std::string>();
    translation.created_at = json.at("created_at").get<std::string>();
    translation.type = json.at("type").get<std::string>();
    if (json.contains("error") && json.at("error").is_object())
    {
        translation.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("data") == true && json.at("data").is_object())
    {
        auto data = Translation::Data{};
        data.utterance_id = json.at("data").at("utterance_id").get<std::string>();
        data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
        data.original_language = json.at("data").at("original_language").get<std::string>();
        data.target_language = json.at("data").at("target_language").get<std::string>();
        data.translated_utterance = Utterance::fromJson(json.at("data").at("translated_utterance"));
        translation.data = data;
    }
    return translation;
}

NamedEntityRecognition::Data::Result gladiapp::v2::ws::response::NamedEntityRecognition::Data::Result::fromJson(const nlohmann::json &json)
{
    Result result;
    result.entity_type = json.at("entity_type").get<std::string>();
    result.text = json.at("text").get<std::string>();
    result.start = json.at("start").get<double>();
    result.end = json.at("end").get<double>();
    return result;
}

NamedEntityRecognition gladiapp::v2::ws::response::NamedEntityRecognition::fromJson(const nlohmann::json &json)
{
    NamedEntityRecognition recognition;
    recognition.session_id = json.at("session_id").get<std::string>();
    recognition.created_at = json.at("created_at").get<std::string>();
    recognition.type = json.at("type").get<std::string>();
    if (json.contains("error") && json.at("error").is_object())
    {
        recognition.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("data") && json.at("data").is_object())
    {
        auto data = NamedEntityRecognition::Data{};
        data.utterance_id = json.at("data").at("utterance_id").get<std::string>();
        data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
        for (const auto &resultJson : json.at("data").at("results"))
        {
            data.results.push_back(NamedEntityRecognition::Data::Result::fromJson(resultJson));
        }
        recognition.data = data;
    }
    return recognition;
}

gladiapp::v2::ws::response::SentimentAnalysis::Data::Result gladiapp::v2::ws::response::SentimentAnalysis::Data::Result::fromJson(const nlohmann::json &json) {
    Result result;
    result.sentiment = json.at("sentiment").get<std::string>();
    result.emotion = json.at("emotion").get<std::string>();
    result.text = json.at("text").get<std::string>();
    result.start = json.at("start").get<double>();
    result.end = json.at("end").get<double>();
    result.channel = json.at("channel").get<double>();
    return result;
}

SentimentAnalysis gladiapp::v2::ws::response::SentimentAnalysis::fromJson(const nlohmann::json &json)
{
    SentimentAnalysis analysis;
    analysis.session_id = json.at("session_id").get<std::string>();
    analysis.created_at = json.at("created_at").get<std::string>();
    analysis.type = json.at("type").get<std::string>();
    if (json.contains("data") && json.at("data").is_object())
    {
        auto data = SentimentAnalysis::Data{};
        data.utterance_id = json.at("data").at("utterance_id").get<std::string>();
        data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
        for (const auto &resultJson : json.at("data").at("results"))
        {
            data.results.push_back(SentimentAnalysis::Data::Result::fromJson(resultJson));
        }
        analysis.data = data;
    }
    return analysis;
}

LifecycleEvent gladiapp::v2::ws::response::LifecycleEvent::fromJson(const nlohmann::json &json)
{
    LifecycleEvent event;
    event.session_id = json.at("session_id").get<std::string>();
    event.created_at = json.at("created_at").get<std::string>();
    event.type = json.at("type").get<std::string>();
    return event;
}

Acknowledgment gladiapp::v2::ws::response::Acknowledgment::fromJson(const nlohmann::json &json)
{
    Acknowledgment acknowledgment;
    acknowledgment.session_id = json.at("session_id").get<std::string>();
    acknowledgment.created_at = json.at("created_at").get<std::string>();
    acknowledgment.type = json.at("type").get<std::string>();
    acknowledgment.acknowledged = json.at("acknowledged").get<bool>();
    return acknowledgment;
}

Summarization gladiapp::v2::ws::response::Summarization::fromJson(const nlohmann::json &json)
{
    Summarization summarization;
    summarization.session_id = json.at("session_id").get<std::string>();
    summarization.created_at = json.at("created_at").get<std::string>();
    summarization.type = json.at("type").get<std::string>();
    if (json.contains("error") && json.at("error").is_object())
    {
        summarization.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("data") && json.at("data").is_object())
    {
        auto data = Summarization::Data{};
        data.results = json.at("data").at("results").get<std::string>();
        summarization.data = data;
    }
    return summarization;
}
