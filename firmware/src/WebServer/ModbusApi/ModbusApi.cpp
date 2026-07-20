#include "ModbusApi.h"
#include <Config/ConfigManager.h>
#include <Common/Network.h>
#include <Common/Json.h>
#include <Modbus/ModbusManager.h>
#include "Common/Enum.h"

void registerModbusApi(AsyncWebServer& server) {
    server.on("/api/modbus/config", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleGetModbusConfig(request);
    });

    server.addHandler(new AsyncCallbackJsonWebHandler(
        "/api/modbus/config", [](AsyncWebServerRequest* request, JsonVariant& json) {
            handleUpdateModbusConfig(request, json);
        }));

    Serial.println("Modbus API registered");
}

void handleGetModbusConfig(AsyncWebServerRequest* request) {
    JsonDocument doc;

    serializeVector(doc["devices"], ConfigManager::config.modbusDevices);
    doc["updateStatus"] = Enum::toString(ModbusManager::updateStatus);

    String response;
    serializeJson(doc, response);

    Serial.println("Modbus Config: " + String(ConfigManager::config.modbusDevices.size()) +
                   " devices");
    Serial.printf("\nFetch Modbus Config: %s\n", doc.as<String>().c_str());

    request->send(200, "application/json", response);
}

void handleUpdateModbusConfig(AsyncWebServerRequest* request, JsonVariant& json) {
    std::vector<ModbusDevice> devices;
    deserializeVector(json["devices"], devices);

    Serial.println("Recieved Modbus Config: " + String(devices.size()) + " devices");
    serializeJson(json, Serial);
    Serial.println();

    ModbusManager::requestUpdate(devices);
    Response::success(request, 202, "OK");
}