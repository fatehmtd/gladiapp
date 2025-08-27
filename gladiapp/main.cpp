#include <iostream>
#include <spdlog/spdlog.h>
#include <future>
#include "./include/gladiapp/gladiapp_rest.hpp"

constexpr const char* tempApiKey = "a273934e-8ff7-4814-a41a-ba507614743d";

int main(int ac, char** av) {

    gladiapp::v2::GladiaRestClient client(tempApiKey);

    // prepare the async upload
    std::future<gladiapp::v2::response::UploadResponse> uploadFuture = std::async([&client]() {
        try {
            return client.upload("/Users/fateh/Documents/testing.wav");
        } catch (const std::exception& e) {
            spdlog::error("Error occurred during upload: {}", e.what());
            return gladiapp::v2::response::UploadResponse();
        }
    });

    // Do other work here while upload is happening
    spdlog::info("Upload started, doing other work...");

    try {
        // Wait for upload to complete and get result
        gladiapp::v2::response::UploadResponse response = uploadFuture.get();
        spdlog::info("response: {}", response.toString());
    } catch (const std::exception& e) {
        spdlog::error("Error occurred: {}", e.what());
        return -1;
    }

    return 0;
}