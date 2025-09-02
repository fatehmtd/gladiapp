#include <iostream>
#include <spdlog/spdlog.h>
#include <future>
#include <gladiapp/gladiapp_rest.hpp>

constexpr const char *tempApiKey = "a273934e-8ff7-4814-a41a-ba507614743d";

int main(int ac, char **av)
{
    spdlog::info("Starting GladiaRestClient...");
    gladiapp::v2::GladiaRestClient client(tempApiKey);

    // prepare the async upload
    std::future<gladiapp::v2::response::UploadResponse> uploadFuture = std::async([&client, av]()
                                                                                  {
        try {
            // Get the directory where the executable is located
            std::string executableDir = std::filesystem::path(av[0]).parent_path().string();
            std::string audioFilePath = (std::filesystem::path(executableDir) / "testing.wav").string();
            return client.upload(audioFilePath);
        } catch (const std::exception& e) {
            spdlog::error("Error occurred during upload: {}", e.what());
            return gladiapp::v2::response::UploadResponse();
        } });

    // Do other work here while upload is happening
    spdlog::info("Upload started, doing other work...");

    try
    {
        // Wait for upload to complete and get result
        gladiapp::v2::response::UploadResponse response = uploadFuture.get();
        spdlog::info("Upload complete, response: {}", response.toString());

        gladiapp::v2::request::TranscriptionRequest request;
        request.audio_url = response.audio_url;
        request.diarization = true;
        request.moderation = true;
        request.subtitles = true;
        request.sentences = true;
        request.subtitles_config = gladiapp::v2::request::TranscriptionRequest::SubtitlesConfig();
        request.subtitles_config->formats = {gladiapp::v2::request::TranscriptionRequest::SubtitlesConfig::Format::VTT,
                                             gladiapp::v2::request::TranscriptionRequest::SubtitlesConfig::Format::SRT};

        request.translation = true;
        request.translation_config.target_languages = {"fr", "de", "es"};

        gladiapp::v2::response::TranscriptionError transcriptionError;

        spdlog::info("Creating transcription job...");

        gladiapp::v2::response::TranscriptionJobResponse transcriptionJobResponse = client.preRecorded(request, &transcriptionError);
        if (transcriptionError.status_code == 0)
        {
            spdlog::info("Transcription job created, ID: {}", transcriptionJobResponse.id);

            std::string status = "";

            while (status != "done" && status != "error")
            {
                gladiapp::v2::response::TranscriptionResult result = client.getResult(transcriptionJobResponse.id);
                spdlog::info("Transcription result: {}", result.status);
                status = result.status;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }

            spdlog::info("Transcription job completed with status: {}", status);

            spdlog::info("getting transcription results...");
            gladiapp::v2::request::ListResultsQuery query;
            query.offset = 0;
            query.limit = 1000;
            auto results = client.getResults(query);

            spdlog::info("Deleting transcription {} results", results.items.size());

            for (const auto &result : results.items)
            {
                spdlog::info("deleting result: {}", result.id);
                client.deleteResult(result.id);
            }
        }
        else
        {
            spdlog::error("Error occurred during transcription job creation: {}", transcriptionError.message);
            spdlog::error("Status code: {}", transcriptionError.status_code);

            // If validation_errors is a vector/array, iterate through it
            if (!transcriptionError.validation_errors.empty())
            {
                spdlog::error("Validation errors:");
                for (const auto &error : transcriptionError.validation_errors)
                {
                    spdlog::error("  - {}", error);
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error occurred: {}", e.what());
        return -1;
    }

    return 0;
}