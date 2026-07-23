#include "ModbusApi.h"
#include <Config/ConfigManager.h>
#include <Common/Network.h>
#include <Common/Json.h>
#include <Modbus/ModbusManager.h>
#include "Common/Enum.h"
#include <Common/Logger.h>

namespace ModbusApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/modbus/config", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetConfig(request);
        });

        server.on("/api/modbus/values", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetValues(request);
        });

        server.addHandler(new AsyncCallbackJsonWebHandler(
            "/api/modbus/config", [](AsyncWebServerRequest* request, JsonVariant& json) {
                handleUpdateConfig(request, json);
            }));

        Log.println("Modbus API registered");
    }

    void handleGetConfig(AsyncWebServerRequest* request) {
        JsonDocument doc;

        serializeVector(doc["devices"], ConfigManager::config.modbusDevices);
        doc["updateStatus"] = Enum::toString(ModbusManager::updateStatus);

        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }

    void handleGetValues(AsyncWebServerRequest* request) {
        JsonDocument doc;
        ModbusManager::getValues(doc);

        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }

    void handleUpdateConfig(AsyncWebServerRequest* request, JsonVariant& json) {
        std::vector<ModbusDevice> devices;
        deserializeVector(json["devices"], devices);

        Log.println("Recieved Modbus Config: " + String(devices.size()) + " devices");
        serializeJson(json, Log);
        Log.println();

        ModbusManager::requestUpdate(devices);
        Response::success(request, 202, "OK");
    }
}  // namespace ModbusApi