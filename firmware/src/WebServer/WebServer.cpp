#include "WebServer.h"
#include <PsychicHttp.h>
#include <LittleFS.h>
#include "NetworkApi/NetworkApi.h"
#include "MqttApi/MqttApi.h"
#include "ConfigApi/ConfigApi.h"
#include "ModbusApi/ModbusApi.h"
#include "UpdateApi/UpdateApi.h"
#include "SystemApi/SystemApi.h"
#include "WebSocket.h"
#include <Common/Logger.h>

namespace WebServer {
    PsychicHttpServer server(80);
    fs::LittleFSFS WebFS;

    String WEBSITE_VERSION = "0.0.0";
    static void setWebsiteVersion() {
        File file = WebFS.open("/version.json", "r");
        if (file) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, file);
            file.close();

            if (error) {
                Log.printf("Failed to deserialize version.json: %s\n", error.c_str());
                return;
            }

            WEBSITE_VERSION = doc["version"] | "0.0.0";
            Log.println("Website version: " + WEBSITE_VERSION);
        } else {
            Log.println("Failed to open version.json");
        }
    }

    bool start() {
        Log.println("Starting web server");

        bool fsMounted = WebFS.begin(false, "/website", 10, "website");

        if (!fsMounted) {
            Log.println("LittleFS Mount Failed");
            return false;
        }

        setWebsiteVersion();

        registerApis();

        server.serveStatic("/_app", WebFS, "/_app")
            ->setCacheControl("public, max-age=31536000, immutable");

        server.serveStatic("/roboto.woff2", WebFS, "/roboto.woff2")
            ->setCacheControl("public, max-age=31536000, immutable");

        server.serveStatic("/", WebFS, "/")
            ->setDefaultFile("index.html")
            ->setCacheControl("no-cache");

        server.onNotFound([](PsychicRequest* request, PsychicResponse* response) -> esp_err_t {
            if (request->path().startsWith("/api/")) {
                return response->send(404);
            }

            PsychicFileResponse file(response, WebFS, "/index.html");
            file.addHeader("Cache-Control", "no-cache");
            return file.send();
        });

        server.begin();

        Log.println("Web server started");
        return true;
    }

    void registerApis() {
        WebSocket::setup(server);
        NetworkApi::registerApi(server);
        MqttApi::registerApi(server);
        ConfigApi::registerApi(server);
        ModbusApi::registerApi(server);
        UpdateApi::registerApi(server);
        SystemApi::registerApi(server);
    }

    bool stop() {
        server.end();
        Log.println("Web server stopped");
        return true;
    }

    size_t getUsedBytes() {
        return WebFS.usedBytes();
    }

    size_t getTotalBytes() {
        return WebFS.totalBytes();
    }

}  // namespace WebServer