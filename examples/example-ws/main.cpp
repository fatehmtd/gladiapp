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
    initRequest.endpointing = 1.5;
    
    initRequest.post_processing = request::InitializeSessionRequest::PostProcessing{};
    initRequest.post_processing->summarization = true;
    request::InitializeSessionRequest::PostProcessing::SummarizationConfig summarizationConfig;
    summarizationConfig.type = request::InitializeSessionRequest::PostProcessing::SummarizationConfig::Type::GENERAL;
    initRequest.post_processing->summarization_config = summarizationConfig;

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
    }

    if (session == nullptr)
    {
        spdlog::error("Failed to create WebSocket session.");
        return 1;
    }

    if (session->connectAndStart())
    {
        spdlog::info("WebSocket session started successfully.");

        // Send audio data in chunks
        size_t chunkSize = 3200; // e.g., 100ms of audio
        for (size_t offset = 0; offset < audioData.size(); offset += chunkSize)
        {
            size_t currentChunkSize = std::min(chunkSize, audioData.size() - offset);
            if (!session->sendAudioBinary(audioData.data() + offset, static_cast<int>(currentChunkSize)))
            {
                spdlog::error("Failed to send audio chunk.");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate real-time sending
        }

        std::this_thread::sleep_for(std::chrono::seconds(200));

        session->sendStopSignal();
        session->disconnect();
    }
    else
    {
        spdlog::error("Failed to connect and start WebSocket session.");
    }

    return 0;
}