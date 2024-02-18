#include <iostream>
#include <cstring>
#include <Encryption.h>
#include "../keyauth/curl/curl.h"
inline char* jsoncontent(const char* text)
{
    // Assuming text is not null
    if (text == nullptr) {
        return 0;  // Handle null input
    }

    // Construct the JSON string with the provided text
    const char* json_template = R"({
        "content": "%s"
    })";

    // Calculate the length of the final JSON string
    size_t json_length = std::snprintf(nullptr, 0, json_template, text) + 1;

    char* json_data = new char[json_length];
    std::snprintf(json_data, json_length, json_template, text);

    return json_data;
}
static size_t writeCallback(void*, size_t size, size_t nmemb, void*) {
    // This function will be called to handle the response data
    return size * nmemb;
}
inline static bool discord(const char* text)
{
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the Discord webhook URL
        const char* webhookUrl = Encrypt("https://discord.com/api/webhooks/1193485737564311622/WGijeTcKg-sWZcws3rdV01-GsUAByfPW9X1TepI-Srgyd6CM5WxqX-XD1WeQveFLNSFZ");

        // Set the request headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, Encrypt("Content-Type: application/json"));
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the request body
        const char* json_data = jsoncontent(text);
        std::snprintf(nullptr, 0, json_data, text) + 1;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

        // Perform the request
        curl_easy_setopt(curl, CURLOPT_URL, webhookUrl);
        CURLcode res = curl_easy_perform(curl);

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        // Inform user about the completion
        if (res == CURLE_OK) {
            return true;
        }
        else {
            return false;
        }
    }

    return 0;
}