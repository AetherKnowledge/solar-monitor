#include "ModbusApi.h"
#include <Config/ConfigManager.h>
#include <Common/Network.h>
#include <Common/Json.h>
#include <Modbus/ModbusManager.h>
#include "ArduinoJson/Json/JsonSerializer.hpp"
#include "Common/Enum.h"
#include <Common/Logger.h>
#include <esp_heap_caps.h>
#include <algorithm>

namespace ModbusApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/modbus/devices/status", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetStatus(request);
        });

        server.on("/api/modbus/devices", HTTP_GET, [](AsyncWebServerRequest* request) {
            if (!request->hasParam("id")) {
                handleGetDevices(request);
                return;
            }

            handleGetDevice(request, request->getParam("id")->value());
        });

        server.on("/api/modbus/values", HTTP_GET, [](AsyncWebServerRequest* request) {
            if (!request->hasParam("id")) {
                Response::error(request, 400, "Missing device id");
                return;
            }

            handleGetValues(request, request->getParam("id")->value());
        });

        server.addHandler(new AsyncCallbackJsonWebHandler(
            "/api/modbus/devices", [](AsyncWebServerRequest* request, JsonVariant& json) {
                if (!request->hasParam("id")) {
                    Response::error(request, 400, "Missing device id");
                    return;
                }

                handleUpdateDevice(request, json, request->getParam("id")->value());
            }));

        Log.println("Modbus API registered");
    }

    void handleGetDevices(AsyncWebServerRequest* request) {
        JsonDocument doc;

        serializeVector(
            doc["devices"], ConfigManager::config.modbusDevices, ModbusDevice::InfoSerializer());

        Log.println("Sending Modbus Devices: " +
                    String(ConfigManager::config.modbusDevices.size()) + " devices");

        Response::sendJson(request, doc);
    }

    void handleGetStatus(AsyncWebServerRequest* request) {
        JsonDocument doc;

        doc["updateStatus"] = Enum::toString(ModbusManager::updateStatus);

        Response::sendJson(request, doc);
    }

    void handleGetDevice(AsyncWebServerRequest* request, String id) {
        JsonDocument doc;

        const ModbusDevice* device = ConfigManager::config.getDeviceById(id);
        if (!device) {
            Response::error(request, 404, "Device not found");
            return;
        }
        device->toJson(doc["device"].to<JsonObject>());

        Response::sendJson(request, doc);
    }

    void handleGetValues(AsyncWebServerRequest* request, String id) {
        JsonDocument doc;
        ModbusManager::getValues(doc, id);
        Response::sendJson(request, doc);
    }

    void handleUpdateDevice(AsyncWebServerRequest* request, JsonVariant& json, String id) {
        auto device = ConfigManager::config.getDeviceById(id);
        if (!device) {
            Response::error(request, 404, "Device not found");
            return;
        }

        Log.println("Recieved Modbus Update for device: " + id);

        ModbusManager::requestUpdate(id, json);
        Response::success(request, 202, "OK");
    }
}  // namespace ModbusApi