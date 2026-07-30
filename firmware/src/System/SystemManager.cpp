#include "SystemManager.h"
#include <Arduino.h>
#include <esp_system.h>
#include <Common/Logger.h>
#include <Display/DisplayManager.h>
#include <Modbus/VirtualSensorManager.h>
#include <Config/ConfigManager.h>
#include "SystemTypes.h"
#include <WebServer/WebServer.h>
#include <esp_heap_caps.h>
#include <Mqtt/MqttManager.h>
#include <esp_ota_ops.h>

namespace SystemManager {
    volatile bool requestedRestart = false;
    unsigned long lastBuzz = 0;
    bool buzzerActive = false;

    SystemDevice systemDevice =
        makeDevice("solar-monitor", "Solar Monitor", "Solar Monitor", "Solar Monitor");
    SystemSensor uptimeSensor =
        makeSensor(systemDevice.discovery, "System Uptime", "uptime", "duration", "s");
    SystemSensor ramUsageSensor =
        makeSensor(systemDevice.discovery, "Ram Usage", "ram_usage", "data_size", "KiB");
    SystemSensor ramTotalSensor =
        makeSensor(systemDevice.discovery, "Ram Total", "ram_total", "data_size", "KiB");
    SystemSensor ramLargestFreeBlockSensor = makeSensor(systemDevice.discovery,
                                                        "Ram Largest Free Block",
                                                        "ram_largest_free_block",
                                                        "data_size",
                                                        "KiB");
    SystemSensor temperatureSensor =
        makeSensor(systemDevice.discovery, "Temperature", "temperature", "temperature", "°C");
    SystemSensor flashUsedSensor =
        makeSensor(systemDevice.discovery, "Flash Used", "flash_used", "data_size", "MiB");
    SystemSensor flashTotalSensor =
        makeSensor(systemDevice.discovery, "Flash Total", "flash_total", "data_size", "MiB");

    SystemSensor codeStorageUsedSensor = makeSensor(
        systemDevice.discovery, "Code Storage Used", "code_storage_used", "data_size", "KiB");

    SystemSensor codeStorageTotalSensor = makeSensor(
        systemDevice.discovery, "Code Storage Total", "code_storage_total", "data_size", "KiB");

    SystemSensor webStorageUsedSensor = makeSensor(
        systemDevice.discovery, "Web Storage Used", "web_storage_used", "data_size", "KiB");
    SystemSensor webStorageTotalSensor = makeSensor(
        systemDevice.discovery, "Web Storage Total", "web_storage_total", "data_size", "KiB");
    SystemSensor configStorageUsedSensor = makeSensor(
        systemDevice.discovery, "Config Storage Used", "config_storage_used", "data_size", "KiB");
    SystemSensor configStorageTotalSensor = makeSensor(
        systemDevice.discovery, "Config Storage Total", "config_storage_total", "data_size", "KiB");

    SystemControl restartControl;

    void setup() {
        setupSensors();
        setupControls();
        systemDevice.generateTopics();
        systemDevice.initialized = true;
    }

    void setupSensors() {
        systemDevice.sensors.clear();
        systemDevice.sensors.reserve(14);

        systemDevice.sensors.push_back(uptimeSensor);
        systemDevice.sensors.push_back(ramUsageSensor);
        systemDevice.sensors.push_back(ramTotalSensor);
        systemDevice.sensors.push_back(ramLargestFreeBlockSensor);
        systemDevice.sensors.push_back(temperatureSensor);
        systemDevice.sensors.push_back(flashUsedSensor);
        systemDevice.sensors.push_back(flashTotalSensor);
        systemDevice.sensors.push_back(codeStorageUsedSensor);
        systemDevice.sensors.push_back(codeStorageTotalSensor);
        systemDevice.sensors.push_back(webStorageUsedSensor);
        systemDevice.sensors.push_back(webStorageTotalSensor);
        systemDevice.sensors.push_back(configStorageUsedSensor);
        systemDevice.sensors.push_back(configStorageTotalSensor);
    }

    void setupControls() {
        systemDevice.controls.clear();
        systemDevice.controls.reserve(1);

        restartControl.discovery.name = "Restart Device";
        restartControl.discovery.uniqueId = systemDevice.discovery.identifier + "_restart";
        restartControl.discovery.commandTopic = systemDevice.discovery.identifier + "/restart";
        restartControl.callback = [](const Device& device, const String& payload) {
            requestRestart();
            return true;
        };

        systemDevice.controls.push_back(restartControl);
    }

    void loop() {
        // Handle system tasks
        if (requestedRestart) {
            DisplayManager::showLoadingSpinner("Restarting...");
            // Perform restart logic
            requestedRestart = false;

            VirtualSensorManager::savePersistence(ConfigManager::config.modbusDevices);
            Log.println("Restarting...");

            delay(1000);
            ESP.restart();
        }

        static uint32_t lastPoll = millis();
        const uint32_t now = millis();
        const uint32_t elapsed = now - lastPoll;
        if (elapsed < 5000)
            return;

        lastPoll = now;

        uptimeSensor.value = millis() / 1000.0;

        ramUsageSensor.value = (ESP.getHeapSize() - ESP.getFreeHeap()) / 1024.0;
        ramTotalSensor.value = ESP.getHeapSize() / 1024.0;
        ramLargestFreeBlockSensor.value =
            heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT) / 1024.0;

        const esp_partition_t* runningPartition = esp_ota_get_running_partition();

        if (runningPartition) {
            codeStorageTotalSensor.value = runningPartition->size / 1024.0;
            codeStorageUsedSensor.value =
                (ESP.getSketchSize() + 1023) / 1024.0;  // Round up to nearest KiB
        }

        webStorageUsedSensor.value = WebServer::getUsedBytes() / 1024.0;
        webStorageTotalSensor.value = WebServer::getTotalBytes() / 1024.0;

        configStorageUsedSensor.value = ConfigManager::ConfigFS.usedBytes() / 1024.0;
        configStorageTotalSensor.value = ConfigManager::ConfigFS.totalBytes() / 1024.0;

        for (auto& ref : systemDevice.sensors) {
            auto& sensor = ref.get();

            if (sensor.discovery.stateTopic.length() > 0 &&
                sensor.discovery.uniqueId.length() > 0) {
                MqttManager::publish(sensor.discovery.stateTopic, String(sensor.value), true);
            }
        }

        const size_t codeUsed = ESP.getSketchSize();
        const size_t webUsed = WebServer::getUsedBytes();
        const size_t configUsed = ConfigManager::ConfigFS.usedBytes();

        const size_t totalUsed = codeUsed + webUsed + configUsed;

        flashUsedSensor.value = totalUsed / (1024.0 * 1024.0);
        flashTotalSensor.value = ESP.getFlashChipSize() / (1024.0 * 1024.0);
    }

    void requestRestart() {
        requestedRestart = true;
    }
}  // namespace SystemManager