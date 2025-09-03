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
    for (const auto &wordJson : json.at("words")) {
        utterance.words.push_back(Utterance::Word::fromJson(wordJson));
    }
    utterance.text = json.at("text").get<std::string>();
    if (json.contains("speaker")) {
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
    if (json.contains("error")) {
        translation.error = Error::fromJson(json.at("error"));
    }
    translation.data.utterance_id = json.at("data").at("utterance_id").get<std::string>();
    translation.data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
    translation.data.original_language = json.at("data").at("original_language").get<std::string>();
    translation.data.target_language = json.at("data").at("target_language").get<std::string>();
    translation.data.translated_utterance = Utterance::fromJson(json.at("data").at("translated_utterance"));
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
    if (json.contains("error")) {
        recognition.error = Error::fromJson(json.at("error"));
    }
    recognition.data.utterance_id = json.at("data").at("utterance_id").get<std::string>();
    recognition.data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
    for (const auto &resultJson : json.at("data").at("results")) {
        recognition.data.results.push_back(NamedEntityRecognition::Data::Result::fromJson(resultJson));
    }
    return recognition;
}
