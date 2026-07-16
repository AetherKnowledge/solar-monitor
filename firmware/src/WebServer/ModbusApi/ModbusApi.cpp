#include "ModbusApi.h"
#include <Config/Config.h>

void registerModbusApi(AsyncWebServer &server)
{
    server.on("/api/config/modbus", HTTP_GET,
              [](AsyncWebServerRequest *request)
              { handleGetModbusConfig(request); });

    Serial.println("Modbus API registered");
}

void handleGetModbusConfig(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    for (const auto &device : config.modbusDevices)
    {
        device.toJson(array.add<JsonObject>());
    }

    String response;
    serializeJson(doc, response);

    Serial.println("Modbus Config: " + String(config.modbusDevices.size()) + " devices");

    request->send(200, "application/json", response);
}