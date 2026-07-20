#include "ModbusApi.h"
#include <Config/Config.h>
#include <Common/Network.h>
#include <Common/Json.h>

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

    serializeVector(doc, config.modbusDevices);

    String response;
    serializeJson(doc, response);

    Serial.println("Modbus Config: " + String(config.modbusDevices.size()) + " devices");
    Serial.printf("\nFetch Modbus Config: %s\n", doc.as<String>().c_str());

    request->send(200, "application/json", response);
}

void handleUpdateModbusConfig(AsyncWebServerRequest* request, JsonVariant& json) {
    std::vector<ModbusDevice> devices;
    deserializeVector(json, devices);

    Serial.println("Recieved Modbus Config: " + String(devices.size()) + " devices");
    serializeJson(json, Serial);
    Serial.println();

    if (updateModbusConfig(devices)) {
        Serial.println("Modbus Config updated successfully");
        Response::success(request, 202, "OK");
    } else {
        Serial.println("Failed to update Modbus Config");
        Response::error(request, 500, "Failed to update Modbus Config");
    }
}