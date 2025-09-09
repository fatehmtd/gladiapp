#include <iostream>
#include <spdlog/spdlog.h>
#include <future>
#include <fstream>
#include <gladiapp/gladiapp_ws.hpp>
#include <gladiapp/gladiapp_error.hpp>

constexpr const char *tempApiKey = "a273934e-8ff7-4814-a41a-ba507614743d";

using namespace gladiapp::v2::ws;

std::vector<uint8_t> readWAVFile(const std::string &filePath)
{
    std::vector<uint8_t> buffer;
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    // skip the wav header (first 44 bytes)
    file.seekg(44, std::ios::beg);
    buffer = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
    file.close();
    return std::move(buffer);
}

int main(int ac, char **av)
{
    spdlog::info("Reading WAV file...");
    std::vector<uint8_t> audioData = readWAVFile("testing.wav");
    spdlog::info("Read {} bytes of audio data from WAV file.", audioData.size());
    spdlog::info("Starting GladiaWebsocketClient...");
    gladiapp::v2::ws::GladiaWebsocketClient client(tempApiKey);

    gladiapp::v2::response::TranscriptionError error;
    gladiapp::v2::ws::request::InitializeSessionRequest initRequest;
    initRequest.channels = 1;
    initRequest.bit_depth = request::InitializeSessionRequest::BitDepth::BIT_DEPTH_16;
    initRequest.encoding = request::InitializeSessionRequest::Encoding::WAV_PCM;
    initRequest.region = request::InitializeSessionRequest::Region::US_WEST;
    initRequest.sample_rate = request::InitializeSessionRequest::SampleRate::SAMPLE_RATE_16000;
    initRequest.model = request::InitializeSessionRequest::Model::SOLARIA_1;
    initRequest.endpointing = 0.15;
    initRequest.maximum_duration_without_endpointing = 60;

    // Realtime processing
    request::InitializeSessionRequest::RealtimeProcessing realtimeProcessing;

    // Translation
    realtimeProcessing.translation = true;
    request::InitializeSessionRequest::RealtimeProcessing::TranslationConfig translationConfig;
    translationConfig.model = request::InitializeSessionRequest::RealtimeProcessing::TranslationConfig::Model::ENHANCED;
    translationConfig.target_languages = {"fr", "es"};
    translationConfig.lipsync = false;
    translationConfig.informal = true;
    realtimeProcessing.translation_config = translationConfig;
    initRequest.realtime_processing = realtimeProcessing;
    realtimeProcessing.named_entity_recognition = true;

    // Post processing, Summarization
    request::InitializeSessionRequest::PostProcessing postProcessing;
    postProcessing.summarization = true;
    request::InitializeSessionRequest::PostProcessing::SummarizationConfig summarizationConfig;
    summarizationConfig.type = request::InitializeSessionRequest::PostProcessing::SummarizationConfig::Type::GENERAL;
    postProcessing.summarization_config = summarizationConfig;
    initRequest.post_processing = postProcessing;

    // Messages config
    request::InitializeSessionRequest::MessagesConfig messagesConfig;
    messagesConfig.receive_final_transcripts = true;
    messagesConfig.receive_speech_events = true;
    messagesConfig.receive_lifecycle_events = true;
    messagesConfig.receive_errors = true;
    messagesConfig.receive_partial_transcripts = true;
    messagesConfig.receive_pre_processing_events = true;
    messagesConfig.receive_realtime_processing_events = true;
    messagesConfig.receive_post_processing_events = true;
    messagesConfig.receive_acknowledgments = false;
    initRequest.messages_config = messagesConfig;

    auto session = std::unique_ptr<GladiaWebsocketClientSession>(client.connect(initRequest, &error));

    if (error.status_code != 0)
    {
        spdlog::error("Error occurred: {}", error.toString());
        return -1;
    }

    if (session == nullptr)
    {
        spdlog::error("Failed to create WebSocket session.");
        return -1;
    }

    if (session->connectAndStart())
    {
        bool waitForCompletion = true;
        spdlog::info("WebSocket session started successfully.");

        session->setOnConnectedCallback([]() {
            spdlog::info("WebSocket connected callback triggered.");
        });
        session->setOnDisconnectedCallback([&waitForCompletion]() {
            spdlog::info("WebSocket disconnected callback triggered.");
            waitForCompletion = false;
        });

        session->setOnSpeechStartedCallback([](const response::SpeechEvent &event) {
            spdlog::info("Callback - Speech Started Event: Session ID: {}, Time: {}, Channel: {}",
                         event.session_id, event.data.time, event.data.channel);
        });
        session->setOnSpeechEndedCallback([](const response::SpeechEvent &event) {
            spdlog::info("Callback - Speech Ended Event: Session ID: {}, Time: {}, Channel: {}",
                         event.session_id, event.data.time, event.data.channel);
        });
        session->setOnTranscriptCallback([](const response::Transcript &transcript) {
            spdlog::info("Callback - Transcript: Session ID: {}, Is Final: {}, Text: {}, Confidence: {}",
                         transcript.session_id, transcript.data.is_final, transcript.data.utterance.text, transcript.data.utterance.confidence);
        });
        session->setOnTranslationCallback([](const response::Translation &translation) {
            if (translation.data.has_value())
            {
                spdlog::info("Callback - Translation: Session ID: {}, Is Final: {}, Text: {}, Confidence: {}",
                             translation.session_id, translation.data->utterance.text, translation.data->translated_utterance.text);
            } else {
                spdlog::error("Callback - Translation: Session ID: {}, message: {}.", translation.session_id, translation.error.has_value() ? translation.error->message.value() : "Unknown error");
            }
        });
        session->setOnNamedEntityRecognitionCallback([](const response::NamedEntityRecognition &ner) {
            if (ner.data.has_value())
            {
                spdlog::info("Callback - Named Entity Recognition: Session ID: {}, Number of Results: {}",
                             ner.session_id, ner.data->results.size());
                for (const auto &result : ner.data->results)
                {
                    spdlog::info(" - Entity Type: {}, Text: {}, Start: {}, End: {}",
                                 result.entity_type, result.text, result.start, result.end);
                }
            } else {
                spdlog::error("Callback - Named Entity Recognition: Session ID: {}, message: {}.", ner.session_id, ner.error.has_value() ? ner.error->message.value() : "Unknown error");
            }
        });
        session->setOnPostTranscriptCallback([](const response::PostTranscript &postTranscript) {
            spdlog::info("Callback - Post Transcript: Session ID: {}, Full Transcript: {}",
                         postTranscript.session_id, postTranscript.data.full_transcript);
        });
        session->setOnFinalTranscriptCallback([](const response::FinalTranscript &finalTranscript) {
            if (finalTranscript.data.transcription.has_value())
            {
                spdlog::info("Callback - Final Transcript: Session ID: {}, Text: {}, Languages: {}",
                             finalTranscript.session_id, finalTranscript.data.transcription->full_transcript, finalTranscript.data.transcription->languages.size());
            } else {
                spdlog::error("Callback - Final Transcript: Session ID: {}, message: {}.", finalTranscript.session_id, finalTranscript.error.has_value() ? finalTranscript.error->message.value() : "Unknown error");
            }
        });

        // Send audio data in chunks
        size_t chunkSize = 3200 * 15; // e.g., 100ms of audio
        spdlog::info("Sending audio data in chunks of {} bytes...", chunkSize);
        // Simulate real-time sending by adding a small delay between chunks
        for (size_t offset = 0; offset < audioData.size(); offset += chunkSize)
        {
            size_t currentChunkSize = std::min(chunkSize, audioData.size() - offset);
            if (!session->sendAudioBinary(audioData.data() + offset, static_cast<int>(currentChunkSize)))
            {
                spdlog::error("Failed to send audio chunk.");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate real-time sending
        }
        spdlog::info("Finished sending audio data. Waiting for processing to complete...");
        spdlog::info("Sending stop signal to indicate end of audio stream.");
        session->sendStopSignal();    
        
        // wait for processing
        while(waitForCompletion) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    else
    {
        spdlog::error("Failed to connect and start WebSocket session.");
    }

    return 0;
}