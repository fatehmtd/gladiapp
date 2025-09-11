#include "gladiapp_ws_response.hpp"
#include <spdlog/spdlog.h>

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
    if(json.contains("session_id") && json.at("session_id").is_string()) {
        event.session_id = json.at("session_id").get<std::string>();
    }
    event.created_at = json.at("created_at").get<std::string>();
    event.type = json.at("type").get<std::string>();
    event.data.time = json.at("data").at("time").get<double>();
    event.data.channel = json.at("data").at("channel").get<int>();
    return event;
}

Error gladiapp::v2::ws::response::Error::fromJson(const nlohmann::json &json)
{
    Error error;
    if (json.contains("status_code"))
    {
        error.status_code = json.at("status_code").get<int>();
    }
    if (json.contains("exception"))
    {
        error.exception = json.at("exception").get<std::string>();
    }
    if (json.contains("message"))
    {
        error.message = json.at("message").get<std::string>();
    }
    return error;
}

gladiapp::v2::ws::response::Word gladiapp::v2::ws::response::Word::fromJson(const nlohmann::json &json)
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
        utterance.words.push_back(Word::fromJson(wordJson));
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
    if(json.contains("session_id") && json.at("session_id").is_string()) {
        transcript.session_id = json.at("session_id").get<std::string>();
    }
    if(json.contains("created_at") && json.at("created_at").is_string()) {
        transcript.created_at = json.at("created_at").get<std::string>();
    }
    if(json.contains("type") && json.at("type").is_string()) {
        transcript.type = json.at("type").get<std::string>();
    }
    if(json.contains("data") && json.at("data").is_object()) {
        transcript.data.id = json.at("data").at("id").get<std::string>();
        transcript.data.is_final = json.at("data").at("is_final").get<bool>();
    }
    transcript.data.utterance = Utterance::fromJson(json.at("data").at("utterance"));
    return transcript;
}

Translation gladiapp::v2::ws::response::Translation::fromJson(const nlohmann::json &json)
{
    Translation translation;
    if(json.contains("session_id") && json.at("session_id").is_string()) {
        translation.session_id = json.at("session_id").get<std::string>();
    }
    if(json.contains("created_at") && json.at("created_at").is_string()) {
        translation.created_at = json.at("created_at").get<std::string>();
    }
    if(json.contains("type") && json.at("type").is_string()) {
        translation.type = json.at("type").get<std::string>();
    }
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
    if(json.contains("entity_type") && json.at("entity_type").is_string()) {
        result.entity_type = json.at("entity_type").get<std::string>();
    }
    if(json.contains("text") && json.at("text").is_string()) {
        result.text = json.at("text").get<std::string>();
    }
    if(json.contains("start") && json.at("start").is_number()) {
        result.start = json.at("start").get<double>();
    }
    if(json.contains("end") && json.at("end").is_number()) {
        result.end = json.at("end").get<double>();
    }
    return result;
}

NamedEntityRecognition gladiapp::v2::ws::response::NamedEntityRecognition::fromJson(const nlohmann::json &json)
{
    NamedEntityRecognition recognition;
    if(json.contains("session_id") && json.at("session_id").is_string()) {
        recognition.session_id = json.at("session_id").get<std::string>();
    }
    if(json.contains("created_at") && json.at("created_at").is_string()) {
        recognition.created_at = json.at("created_at").get<std::string>();
    }
    if(json.contains("type") && json.at("type").is_string()) {
        recognition.type = json.at("type").get<std::string>();
    }
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

gladiapp::v2::ws::response::SentimentAnalysis::Data::Result gladiapp::v2::ws::response::SentimentAnalysis::Data::Result::fromJson(const nlohmann::json &json)
{
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
    if (json.contains("session_id") && json.at("session_id").is_string()) {
        analysis.session_id = json.at("session_id").get<std::string>();
    }
    if (json.contains("created_at") && json.at("created_at").is_string()) {
        analysis.created_at = json.at("created_at").get<std::string>();
    }
    if (json.contains("type") && json.at("type").is_string()) {
        analysis.type = json.at("type").get<std::string>();
    }
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
    if (json.contains("session_id") && json.at("session_id").is_string()) {
        event.session_id = json.at("session_id").get<std::string>();
    }
    if (json.contains("created_at") && json.at("created_at").is_string()) {
        event.created_at = json.at("created_at").get<std::string>();
    }
    if (json.contains("type") && json.at("type").is_string()) {
        event.type = json.at("type").get<std::string>();
    }
    return event;
}

Acknowledgment gladiapp::v2::ws::response::Acknowledgment::fromJson(const nlohmann::json &json)
{
    Acknowledgment acknowledgment;
    if (json.contains("session_id") && json.at("session_id").is_string()) {
        acknowledgment.session_id = json.at("session_id").get<std::string>();
    }
    if (json.contains("created_at") && json.at("created_at").is_string()) {
        acknowledgment.created_at = json.at("created_at").get<std::string>();
    }
    if (json.contains("type") && json.at("type").is_string()) {
        acknowledgment.type = json.at("type").get<std::string>();
    }
    if (json.contains("acknowledged") && json.at("acknowledged").is_boolean()) {
        acknowledgment.acknowledged = json.at("acknowledged").get<bool>();
    }
    return acknowledgment;
}

Summarization gladiapp::v2::ws::response::Summarization::fromJson(const nlohmann::json &json)
{
    Summarization summarization;
    if (json.contains("session_id") && json.at("session_id").is_string()) {
        summarization.session_id = json.at("session_id").get<std::string>();
    }
    if (json.contains("created_at") && json.at("created_at").is_string()) {
        summarization.created_at = json.at("created_at").get<std::string>();
    }
    if (json.contains("type") && json.at("type").is_string()) {
        summarization.type = json.at("type").get<std::string>();
    }
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

gladiapp::v2::ws::response::Transcription::AudioToLLMResult::Result gladiapp::v2::ws::response::Transcription::AudioToLLMResult::Result::fromJson(const nlohmann::json &json)
{
    Result result;    
    result.success = json.at("success").get<bool>();
    result.is_empty = json.at("is_empty").get<bool>();
    result.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        result.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results") && json.at("results").is_array())
    {
        for (const auto &item : json.at("results"))
        {
            Result::ResultPair pair;
            if (item.contains("prompt") && !item.at("prompt").is_null())
            {
                pair.prompt = item.at("prompt").get<std::string>();
            }
            if (item.contains("response") && !item.at("response").is_null())
            {
                pair.response = item.at("response").get<std::string>();
            }
            if (pair.prompt.has_value() || pair.response.has_value())
            {
                if (!result.results.has_value())
                {
                    result.results = std::vector<Result::ResultPair>{};
                }
                result.results->push_back(std::move(pair));
            }
        }
    }
    return result;
}

gladiapp::v2::ws::response::Transcription::AudioToLLMResult gladiapp::v2::ws::response::Transcription::AudioToLLMResult::fromJson(const nlohmann::json &json)
{
    AudioToLLMResult audioToLLMResult;

    audioToLLMResult.success = json.at("success").get<bool>();
    audioToLLMResult.is_empty = json.at("is_empty").get<bool>();
    audioToLLMResult.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        audioToLLMResult.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results") && json.at("results").is_array())
    {
        auto results = std::vector<AudioToLLMResult::Result>{};
        for (const auto &item : json.at("results"))
        {
            results.emplace_back(Result::fromJson(item));
        }
        audioToLLMResult.results = std::move(results);
    }
    return audioToLLMResult;
}

gladiapp::v2::ws::response::Transcription::DisplayMode gladiapp::v2::ws::response::Transcription::DisplayMode::fromJson(const nlohmann::json &json)
{
    DisplayMode displayMode;
    displayMode.success = json.at("success").get<bool>();
    displayMode.is_empty = json.at("is_empty").get<bool>();
    displayMode.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        displayMode.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results") && json.at("results").is_array())
    {
        displayMode.results = json.at("results").get<std::vector<std::string>>();
    }
    return displayMode;
}

gladiapp::v2::ws::response::Transcription::ChapterizationResult gladiapp::v2::ws::response::Transcription::ChapterizationResult::fromJson(const nlohmann::json &json)
{
    ChapterizationResult chapterizationResult;
    chapterizationResult.success = json.at("success").get<bool>();
    chapterizationResult.is_empty = json.at("is_empty").get<bool>();
    chapterizationResult.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        chapterizationResult.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results") && !json.at("results").is_null())
    {
        chapterizationResult.results = json.at("results").get<std::string>();
    }
    return chapterizationResult;
}

gladiapp::v2::ws::response::Transcription::DiarizationResult::Result gladiapp::v2::ws::response::Transcription::DiarizationResult::Result::fromJson(const nlohmann::json &json)
{
    Result result;
    result.start = json.at("start").get<double>();
    result.end = json.at("end").get<double>();
    result.confidence = json.at("confidence").get<double>();
    result.channel = json.at("channel").get<int>();
    if (json.contains("speaker") && !json.at("speaker").is_null())
    {
        result.speaker = json.at("speaker").get<int>();
    }
    for (const auto &wordJson : json.at("words"))
    {
        result.words.push_back(Word::fromJson(wordJson));
    }
    result.text = json.at("text").get<std::string>();
    result.language = json.at("language").get<std::string>();
    return result;
}

gladiapp::v2::ws::response::Transcription::DiarizationResult gladiapp::v2::ws::response::Transcription::DiarizationResult::fromJson(const nlohmann::json &json)
{
    DiarizationResult result;
    result.success = json.at("success").get<bool>();
    result.is_empty = json.at("is_empty").get<bool>();
    result.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        result.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results") && json.at("results").is_array())
    {
        std::vector<DiarizationResult::Result> results;
        for (const auto &item : json.at("results"))
        {
            results.push_back(Result::fromJson(item));
        }
        result.results = std::move(results);
    }
    return result;
}

gladiapp::v2::ws::response::Metadata gladiapp::v2::ws::response::Metadata::fromJson(const nlohmann::json &json)
{
    Metadata metadata;
    metadata.audio_duration = json.at("audio_duration").get<double>();
    metadata.number_of_distinct_channels = json.at("number_of_distinct_channels").get<int>();
    metadata.billing_time = json.at("billing_time").get<double>();
    metadata.transcription_time = json.at("transcription_time").get<double>();
    return metadata;
}

gladiapp::v2::ws::response::Transcription::GenericResult gladiapp::v2::ws::response::Transcription::GenericResult::fromJson(const nlohmann::json &json)
{
    GenericResult result;
    result.success = json.at("success").get<bool>();
    result.is_empty = json.at("is_empty").get<bool>();
    result.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        result.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results"))
    {
        if(json.at("results").is_array()) {
            result.results = json.at("results").get<std::vector<std::string>>();
        }
        else if(json.at("results").is_string()) {
            result.results.push_back(json.at("results").get<std::string>());
        } else {
            spdlog::warn("Unexpected format for GenericResult results field, field \"results\" found with type: {}", json.at("results").type_name());
        }
    }
    return result;
}

gladiapp::v2::ws::response::Transcription::NamedEntityRecognitionResult gladiapp::v2::ws::response::Transcription::NamedEntityRecognitionResult::fromJson(const nlohmann::json &json)
{
    NamedEntityRecognitionResult result;
    result.success = json.at("success").get<bool>();
    result.is_empty = json.at("is_empty").get<bool>();
    result.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        result.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("entity"))
    {
        result.entity = json.at("entity").dump();
    }
    return result;
}

gladiapp::v2::ws::response::Transcription gladiapp::v2::ws::response::Transcription::fromJson(const nlohmann::json &json)
{
    Transcription transcription;
    transcription.full_transcript = json.at("full_transcript").get<std::string>();
    transcription.languages = json.at("languages").get<std::vector<std::string>>();
    if (json.contains("subtitles") && json.at("subtitles").is_array())
    {

        for (const auto &subtitleJson : json.at("subtitles"))
        {
            transcription.subtitles.push_back(Subtitle::fromJson(subtitleJson));
        }
    }
    if (json.contains("utterances") && json.at("utterances").is_array())
    {
        for (const auto &utteranceJson : json.at("utterances"))
        {
            transcription.utterances.push_back(Utterance::fromJson(utteranceJson));
        }
    }
    if (json.contains("summarization") && !json.at("summarization").is_null())
    {
        transcription.summarization = GenericResult::fromJson(json.at("summarization"));
    }
    if (json.contains("moderation") && !json.at("moderation").is_null())
    {
        transcription.moderation = GenericResult::fromJson(json.at("moderation"));
    }
    if (json.contains("named_entity_recognition") && !json.at("named_entity_recognition").is_null())
    {
        transcription.named_entity_recognition = NamedEntityRecognitionResult::fromJson(json.at("named_entity_recognition"));
    }
    if (json.contains("name_consistency") && !json.at("name_consistency").is_null())
    {
        transcription.name_consistency = GenericResult::fromJson(json.at("name_consistency"));
    }
    if (json.contains("custom_spelling") && !json.at("custom_spelling").is_null())
    {
        transcription.custom_spelling = GenericResult::fromJson(json.at("custom_spelling"));
    }
    if (json.contains("speaker_reidentification") && !json.at("speaker_reidentification").is_null())
    {
        transcription.speaker_reidentification = GenericResult::fromJson(json.at("speaker_reidentification"));
    }
    if (json.contains("structured_data_extraction") && !json.at("structured_data_extraction").is_null())
    {
        transcription.structured_data_extraction = GenericResult::fromJson(json.at("structured_data_extraction"));
    }
    if (json.contains("sentiment_analysis") && !json.at("sentiment_analysis").is_null())
    {
        transcription.sentiment_analysis = GenericResult::fromJson(json.at("sentiment_analysis"));
    }
    if (json.contains("audio_to_llm") && !json.at("audio_to_llm").is_null())
    {
        transcription.audio_to_llm = AudioToLLMResult::fromJson(json.at("audio_to_llm"));
    }
    if (json.contains("display_mode") && !json.at("display_mode").is_null())
    {
        transcription.display_mode = DisplayMode::fromJson(json.at("display_mode"));
    }
    if (json.contains("chapters") && !json.at("chapters").is_null())
    {
        transcription.chapters = ChapterizationResult::fromJson(json.at("chapters"));
    }
    if (json.contains("diarization_enhanced") && !json.at("diarization_enhanced").is_null())
    {
        transcription.diarization_enhanced = DiarizationResult::fromJson(json.at("diarization_enhanced"));
    }
    if (json.contains("diarization") && !json.at("diarization").is_null())
    {
        transcription.diarization = DiarizationResult::fromJson(json.at("diarization"));
    }
    return transcription;
}

gladiapp::v2::ws::response::Subtitle gladiapp::v2::ws::response::Subtitle::fromJson(const nlohmann::json &json)
{
    Subtitle subtitle;
    subtitle.format = json.at("format").get<std::string>();
    subtitle.subtitles = json.at("subtitles").get<std::string>();
    return subtitle;
}

gladiapp::v2::ws::response::Chapterization::Data::Chapter gladiapp::v2::ws::response::Chapterization::Data::Chapter::fromJson(const nlohmann::json &json)
{
    Chapter chapter;
    chapter.headline = json.at("headline").get<std::string>();
    chapter.start = json.at("start").get<double>();
    chapter.end = json.at("end").get<double>();
    chapter.summary = json.at("summary").get<std::string>();
    chapter.gist = json.at("gist").get<std::string>();
    chapter.keywords = json.at("keywords").get<std::vector<std::string>>();
    chapter.text = json.at("text").get<std::string>();
    if (json.contains("sentences") && json.at("sentences").is_array())
    {
        for (const auto &sentenceJson : json.at("sentences"))
        {
            chapter.sentences.push_back(Sentence::fromJson(sentenceJson));
        }
    }
    return chapter;
}

gladiapp::v2::ws::response::Chapterization gladiapp::v2::ws::response::Chapterization::fromJson(const nlohmann::json &json)
{
    Chapterization chapterization;
    chapterization.session_id = json.at("session_id").get<std::string>();
    chapterization.created_at = json.at("created_at").get<std::string>();
    chapterization.type = json.at("type").get<std::string>();
    if (json.contains("error") && json.at("error").is_object())
    {
        chapterization.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("data") && json.at("data").is_object())
    {
        auto data = Chapterization::Data{};
        for (const auto &chapterJson : json.at("data").at("results"))
        {
            data.results.push_back(Chapterization::Data::Chapter::fromJson(chapterJson));
        }
        chapterization.data = data;
    }
    return chapterization;
}

gladiapp::v2::ws::response::FinalTranscript gladiapp::v2::ws::response::FinalTranscript::fromJson(const nlohmann::json &json)
{
    FinalTranscript finalTranscript;
    if(json.contains("session_id") && json.at("session_id").is_string()) {
        finalTranscript.session_id = json.at("session_id").get<std::string>();
    }
    if(json.contains("created_at") && json.at("created_at").is_string()) {
        finalTranscript.created_at = json.at("created_at").get<std::string>();
    }
    if(json.contains("type") && json.at("type").is_string()) {
        finalTranscript.type = json.at("type").get<std::string>();
    }
    if (json.contains("error") && json.at("error").is_object())
    {
        finalTranscript.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("data") && json.at("data").is_object())
    {
        auto jsonData = json.at("data");
        FinalTranscript::Data data;
        data.metadata = Metadata::fromJson(jsonData.at("metadata"));
        if (jsonData.contains("transcription") && jsonData.at("transcription").is_object())
        {
            data.transcription = Transcription::fromJson(jsonData.at("transcription"));
        }
        if (jsonData.contains("translation") && jsonData.at("translation").is_object())
        {
            data.translation = Translation::fromJson(jsonData.at("translation"));
        }
        finalTranscript.data = data;
    }
    return finalTranscript;
}

gladiapp::v2::ws::response::PostTranscript gladiapp::v2::ws::response::PostTranscript::fromJson(const nlohmann::json &json)
{
    PostTranscript postTranscript;
    postTranscript.session_id = json.at("session_id").get<std::string>();
    postTranscript.created_at = json.at("created_at").get<std::string>();
    postTranscript.type = json.at("type").get<std::string>();
    if (json.contains("error") && json.at("error").is_object())
    {
        postTranscript.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("data") && json.at("data").is_object())
    {
        PostTranscript::Data data;
        data.full_transcript = json.at("data").at("full_transcript").get<std::string>();
        data.languages = json.at("data").at("languages").get<std::vector<std::string>>();
        auto subtitles = std::vector<Subtitle>{};
        if( json.at("data").contains("subtitles") && json.at("data").at("subtitles").is_array())
        {
            for (const auto &subtitleJson : json.at("data").at("subtitles"))
            {
                subtitles.push_back(Subtitle::fromJson(subtitleJson));
            }
        }
        data.subtitles = subtitles;
        auto utterances = std::vector<Utterance>{};
        for (const auto &utteranceJson : json.at("data").at("utterances"))
        {
            utterances.push_back(Utterance::fromJson(utteranceJson));
        }
        data.utterances = utterances;
        auto sentences = std::vector<Sentence>{};
        if( json.at("data").contains("sentences") && json.at("data").at("sentences").is_array())
        {
            for (const auto &sentenceJson : json.at("data").at("sentences"))
            {
                sentences.push_back(Sentence::fromJson(sentenceJson));
            }
        }
        data.sentences = sentences;
        if (json.at("data").contains("results") && !json.at("data").at("results").is_null())
        {
            data.results = json.at("data").at("results").get<std::vector<std::string>>();
        }
        auto subtitlesVec = std::vector<Subtitle>{};
        if (json.at("data").contains("subtitles") && !json.at("data").at("subtitles").is_null())
        {
            for (const auto &subtitleJson : json.at("data").at("subtitles"))
            {
                subtitlesVec.push_back(Subtitle::fromJson(subtitleJson));
            }
            data.subtitles = subtitlesVec;
        }
        postTranscript.data = data;
    }
    return postTranscript;
}

gladiapp::v2::ws::response::Sentence gladiapp::v2::ws::response::Sentence::fromJson(const nlohmann::json &json)
{
    Sentence sentence;
    sentence.success = json.at("success").get<bool>();
    sentence.is_empty = json.at("is_empty").get<bool>();
    sentence.exec_time = json.at("exec_time").get<double>();
    if (json.contains("error") && json.at("error").is_object())
    {
        sentence.error = Error::fromJson(json.at("error"));
    }
    if (json.contains("results") && json.at("results").is_array())
    {
        sentence.results = json.at("results").get<std::vector<std::string>>();
    }
    return sentence;
}

gladiapp::v2::ws::response::LiveTranscriptionResult::Result gladiapp::v2::ws::response::LiveTranscriptionResult::Result::fromJson(const nlohmann::json &json) {
    Result result;
    result.metadata = Metadata::fromJson(json.at("metadata"));
    if(json.contains("messages") && json.at("messages").is_array()) {
        result.messages = json.at("messages").get<std::vector<std::string>>();
    }
    if(json.contains("transcription") && json.at("transcription").is_object()) {
        result.transcription = Transcription::fromJson(json.at("transcription"));
    }
    if(json.contains("summarization") && json.at("summarization").is_object()) {
        result.summarization = Transcription::SummarizationResult::fromJson(json.at("summarization"));
    }
    if(json.contains("moderation") && json.at("moderation").is_object()) {
        result.moderation = Transcription::ModerationResult::fromJson(json.at("moderation"));
    }
    if(json.contains("named_entity_recognition") && json.at("named_entity_recognition").is_object()) {
        result.named_entity_recognition = Transcription::NamedEntityRecognitionResult::fromJson(json.at("named_entity_recognition"));
    }
    if(json.contains("name_consistency") && json.at("name_consistency").is_object()) {
        result.name_consistency = Transcription::GenericResult::fromJson(json.at("name_consistency"));
    }
    if(json.contains("custom_spelling") && json.at("custom_spelling").is_object()) {
        result.custom_spelling = Transcription::GenericResult::fromJson(json.at("custom_spelling"));
    }
    if(json.contains("speaker_reidentification") && json.at("speaker_reidentification").is_object()) {
        result.speaker_reidentification = Transcription::GenericResult::fromJson(json.at("speaker_reidentification"));
    }
    if(json.contains("structured_data_extraction") && json.at("structured_data_extraction").is_object()) {
        result.structured_data_extraction = Transcription::GenericResult::fromJson(json.at("structured_data_extraction"));
    }
    if(json.contains("sentiment_analysis") && json.at("sentiment_analysis").is_object()) {
        result.sentiment_analysis = Transcription::SentimentAnalysisResult::fromJson(json.at("sentiment_analysis"));
    }
    if(json.contains("audio_to_llm") && json.at("audio_to_llm").is_object()) {
        result.audio_to_llm = Transcription::AudioToLLMResult::fromJson(json.at("audio_to_llm"));
    }
    if(json.contains("display_mode") && json.at("display_mode").is_object()) {
        result.display_mode = Transcription::DisplayMode::fromJson(json.at("display_mode"));
    }
    if(json.contains("chapters") && json.at("chapters").is_object()) {
        result.chapters = Transcription::ChapterizationResult::fromJson(json.at("chapters"));
    }
    if(json.contains("diarization_enhanced") && json.at("diarization_enhanced").is_object()) {
        result.diarization_enhanced = Transcription::DiarizationResult::fromJson(json.at("diarization_enhanced"));
    }
    if(json.contains("diarization") && json.at("diarization").is_object()) {
        result.diarization = Transcription::DiarizationResult::fromJson(json.at("diarization"));
    }    
    // TODO: add translation when available in live transcription
    return result;
}

gladiapp::v2::ws::response::LiveTranscriptionResult gladiapp::v2::ws::response::LiveTranscriptionResult::fromJson(const nlohmann::json &json)
{
    LiveTranscriptionResult liveTranscriptionResult;
    liveTranscriptionResult.id = json.at("id").get<std::string>();
    liveTranscriptionResult.request_id = json.at("request_id").get<std::string>();
    liveTranscriptionResult.status = json.at("status").get<std::string>();
    liveTranscriptionResult.created_at = json.at("created_at").get<std::string>();
    if (json.contains("completed_at") && json.at("completed_at").is_string())
    {
        liveTranscriptionResult.completed_at = json.at("completed_at").get<std::string>();
    }
    liveTranscriptionResult.kind = json.at("kind").get<std::string>();
    if(json.contains("custom_metadata"))  {
        liveTranscriptionResult.custom_metadata = json.at("custom_metadata").dump();
    }
    if(json.contains("error_code") && json.at("error_code").is_number()) {
        liveTranscriptionResult.error_code = json.at("error_code").get<int>();
    }
    if(json.contains("file") && json.at("file").is_object()) {
        auto jsonFile = json.at("file");
        liveTranscriptionResult.file = gladiapp::v2::response::TranscriptionFile::fromJson(jsonFile.dump());
    }
    if(json.contains("request_params") && json.at("request_params").is_object()) {
        liveTranscriptionResult.request_params = RequestParams::fromJson(json.at("request_params"));
    }
    if(json.contains("result") && json.at("result").is_object()) {
        liveTranscriptionResult.result = Result::fromJson(json.at("result"));
    }
    return liveTranscriptionResult;
}
