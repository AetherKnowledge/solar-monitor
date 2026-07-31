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
    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/modbus/devices/status", HTTP_GET, handleGetStatus);

        server.on("/api/modbus/devices",
                  HTTP_GET,
                  [](PsychicRequest* request, PsychicResponse* response) {
                      if (!request->hasParam("id")) {
                          return handleGetDevices(request, response);
                      }

                      return handleGetDevice(request, response, request->getParam("id")->value());
                  });

        server.on("/api/modbus/values", HTTP_GET, handleGetValues);

        server.on("/api/modbus/devices", HTTP_POST, handleUpdateDevice);

        Log.println("Modbus API registered");
    }

    esp_err_t handleGetDevices(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;

        serializeVector(
            doc["devices"], ConfigManager::config.modbusDevices, ModbusDevice::InfoSerializer());

        Log.println("Sending Modbus Devices: " +
                    String(ConfigManager::config.modbusDevices.size()) + " devices");

        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetStatus(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;

        doc["updateStatus"] = Enum::toString(ModbusManager::updateStatus);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetDevice(PsychicRequest* request, PsychicResponse* response, String id) {
        JsonDocument doc;

        const ModbusDevice* device = ConfigManager::config.getDeviceById(id);
        if (!device) {
            return Response::error(response, 404, "Device not found");
        }
        device->toJson(doc["device"].to<JsonObject>());

        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetValues(PsychicRequest* request, PsychicResponse* response) {
        String id = request->getParam("id")->value();

        JsonDocument doc;
        ModbusManager::getValues(doc, id);
        return Response::sendJson(response, doc);
    }

    esp_err_t handleUpdateDevice(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json) {
        if (!request->hasParam("id")) {
            return Response::error(response, 400, "Missing device id");
        }

        String id = request->getParam("id")->value();

        auto device = ConfigManager::config.getDeviceById(id);
        if (!device) {
            return Response::error(response, 404, "Device not found");
        }

        Log.println("Recieved Modbus Update for device: " + id);

        ModbusManager::requestUpdate(id, json);
        return Response::success(response, 202, "OK");
    }
}  // namespace ModbusApi