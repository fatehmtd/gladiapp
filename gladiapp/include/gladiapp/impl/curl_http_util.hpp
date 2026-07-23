#pragma once

#include "../gladiapp.hpp"
#include "../utils.hpp"
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <string>
#include <stdexcept>

namespace gladiapp::v2::curl_util
{
    // Ensures curl_global_init() runs exactly once before any easy handle is created,
    // and curl_global_cleanup() runs at process exit.
    inline void ensureGlobalInit()
    {
        static struct GlobalInit
        {
            GlobalInit() { curl_global_init(CURL_GLOBAL_DEFAULT); }
            ~GlobalInit() { curl_global_cleanup(); }
        } globalInit;
        (void)globalInit;
    }

    struct HttpResponse
    {
        long statusCode = 0;
        std::string body;
    };

    inline size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        auto *out = static_cast<std::string *>(userdata);
        out->append(ptr, size * nmemb);
        return size * nmemb;
    }

    inline std::string buildUrl(const std::string &path)
    {
        return "https://" + std::string(gladiapp::v2::common::HOST) + path;
    }

    // mbedTLS (curl's TLS backend on Android) ships with no built-in trust
    // anchors, so without an explicit CA file every handshake fails with
    // "SSL peer certificate ... was not OK". caFilePath is empty on platforms
    // where curl's platform default trust store is sufficient.
    inline void applyCaFile(CURL *curl, const std::string &caFilePath)
    {
        if (!caFilePath.empty())
        {
            curl_easy_setopt(curl, CURLOPT_CAINFO, caFilePath.c_str());
        }
    }

    // Performs a synchronous HTTPS request against api.gladia.io.
    // method: "GET", "POST" or "DELETE". body/contentType are only used for POST.
    inline HttpResponse performRequest(const std::string &url,
                                       const std::string &method,
                                       const std::string &apiKey,
                                       const std::string &body = "",
                                       const std::string &contentType = "",
                                       const std::string &caFilePath = {})
    {
        ensureGlobalInit();

        CURL *curl = curl_easy_init();
        if (!curl)
        {
            throw std::runtime_error("Failed to initialize curl easy handle");
        }
        applyCaFile(curl, caFilePath);

        struct curl_slist *headerList = nullptr;
        std::string apiKeyHeader = std::string(gladiapp::v2::headers::X_GLADIA_KEY) + ": " + apiKey;
        headerList = curl_slist_append(headerList, apiKeyHeader.c_str());
        if (!contentType.empty())
        {
            headerList = curl_slist_append(headerList, ("Content-Type: " + contentType).c_str());
        }

        HttpResponse response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, gladiapp::v2::common::USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
        // The live session endpoints redirect to a regional host (e.g. api.us-west-1.gladia.io).
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);

        if (method == "POST")
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
        }
        else if (method == "DELETE")
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        }

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::string errorMessage = curl_easy_strerror(res);
            curl_slist_free_all(headerList);
            curl_easy_cleanup(curl);
            throw std::runtime_error("curl request failed: " + errorMessage);
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);

        curl_slist_free_all(headerList);
        curl_easy_cleanup(curl);
        return response;
    }

    // Performs a synchronous multipart/form-data file upload.
    inline HttpResponse performUpload(const std::string &url,
                                      const std::string &apiKey,
                                      const std::string &filePath,
                                      const std::string &fieldName = "audio",
                                      const std::string &fileContentType = "audio/mpeg",
                                      const std::string &caFilePath = {})
    {
        ensureGlobalInit();

        if (!std::filesystem::exists(filePath))
        {
            throw std::runtime_error("Cannot open file: " + filePath);
        }

        CURL *curl = curl_easy_init();
        if (!curl)
        {
            throw std::runtime_error("Failed to initialize curl easy handle");
        }
        applyCaFile(curl, caFilePath);

        struct curl_slist *headerList = nullptr;
        std::string apiKeyHeader = std::string(gladiapp::v2::headers::X_GLADIA_KEY) + ": " + apiKey;
        headerList = curl_slist_append(headerList, apiKeyHeader.c_str());

        curl_mime *mime = curl_mime_init(curl);
        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_name(part, fieldName.c_str());
        curl_mime_filedata(part, filePath.c_str());
        curl_mime_type(part, fileContentType.c_str());

        HttpResponse response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, gladiapp::v2::common::USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);

        spdlog::info("file name: {} file size: {}",
                     std::filesystem::path(filePath).filename().string(),
                     gladiapp::utils::formatBytes(static_cast<long long>(std::filesystem::file_size(filePath))));

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::string errorMessage = curl_easy_strerror(res);
            curl_mime_free(mime);
            curl_slist_free_all(headerList);
            curl_easy_cleanup(curl);
            throw std::runtime_error("curl upload failed: " + errorMessage);
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);

        curl_mime_free(mime);
        curl_slist_free_all(headerList);
        curl_easy_cleanup(curl);
        return response;
    }

    inline std::string urlEncode(const std::string &value)
    {
        ensureGlobalInit();
        CURL *curl = curl_easy_init();
        char *encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.size()));
        std::string result = encoded ? encoded : value;
        if (encoded)
        {
            curl_free(encoded);
        }
        curl_easy_cleanup(curl);
        return result;
    }
}
