#pragma once

#include <laserpants/dotenv/dotenv.h>
#include <spdlog/spdlog.h>

// Helper function to get API key using dotenv-cpp
std::string getApiKey() {
    // Load .env file with Preserve flag (environment variables take precedence)
    dotenv::init(dotenv::Preserve, ".env");
    
    // Try to get API key using dotenv's getenv wrapper with default value
    std::string apiKey = dotenv::getenv("GLADIA_API_KEY", "");
    
    if (!apiKey.empty()) {
        spdlog::info("API key loaded successfully");
        return apiKey;
    }
    
    // Final fallback to demo key for quick testing
    spdlog::warn("No API key found in .env file or environment variable.");
    spdlog::warn("Using demo key. For production:");
    spdlog::warn("1. Create .env file with: GLADIA_API_KEY=your-key");
    spdlog::warn("2. Or set environment: set GLADIA_API_KEY=your-key");

    return "some-random-text-that-is-not-a-real-api-key";
}