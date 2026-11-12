from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
COMPILER = shutil.which("g++") or shutil.which("clang++")
JSON_HEADERS = next((ROOT / ".pio/libdeps").glob("*/ArduinoJson/src/ArduinoJson.h"), None)


@unittest.skipUnless(COMPILER and JSON_HEADERS, "Requires a host C++ compiler and PlatformIO ArduinoJson")
class RegisterPatchTests(unittest.TestCase):
    def test_firmware_applies_and_persists_entity_deletions(self):
        with tempfile.TemporaryDirectory() as directory:
            temp = Path(directory)
            (temp / "Arduino.h").write_text(r'''
#pragma once
#include <algorithm>
#include <concepts>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>
class String : public std::string {
public:
    using std::string::string;
    using std::string::operator=;
    bool isEmpty() const { return empty(); }
};
''')
            source = temp / "test.cpp"
            source.write_text(r'''
#include <Common/Json.h>
#include <cassert>

struct Entity {
    struct Discovery { String uniqueId; String name; } discovery;
    double value = 0;
    int scale = 10;
    Discovery& getDiscovery() { return discovery; }
    const Discovery& getDiscovery() const { return discovery; }
    void fromJson(JsonObject json, bool update = false) {
        discovery.uniqueId = json["discovery"]["unique_id"] | "";
        discovery.name = json["discovery"]["name"] | discovery.name.c_str();
        scale = json["scale"] | scale;
    }
    void toJson(JsonObject json) const {
        json["discovery"]["unique_id"] = std::string(discovery.uniqueId);
        json["discovery"]["name"] = std::string(discovery.name);
        json["scale"] = scale;
    }
};

void apply(const char* payload, std::vector<Entity>& entities) {
    JsonDocument doc;
    assert(!deserializeJson(doc, payload));
    applyVector(doc.as<JsonArray>(), entities);
}

int main() {
    std::vector<Entity> entities;
    apply(R"([
      {"discovery":{"unique_id":"keep","name":"Keep"},"scale":7},
      {"discovery":{"unique_id":"edit","name":"Edit"},"scale":8},
      {"discovery":{"unique_id":"remove","name":"Remove"}}
    ])", entities);
    const char* patch = R"([
      {"discovery":{"unique_id":"edit","name":"Edited"}},
      {"discovery":{"unique_id":"add","name":"Added"}},
      {"discovery":{"unique_id":"remove"},"deleted":true}
    ])";
    apply(patch, entities);
    assert(entities.size() == 3);
    assert(entities[0].discovery.uniqueId == "keep" && entities[0].scale == 7);
    assert(entities[1].discovery.name == "Edited" && entities[1].scale == 8);
    assert(entities[2].discovery.uniqueId == "add");
    apply(patch, entities);
    assert(entities.size() == 3);
    apply(R"([
      {"deleted":true},
      {"discovery":{"unique_id":""},"deleted":true},
      {"discovery":{"unique_id":"unknown"},"deleted":true}
    ])", entities);
    assert(entities.size() == 3);
    JsonDocument saved;
    serializeVector(saved.to<JsonArray>(), entities);
    std::string disk;
    serializeJson(saved, disk);
    assert(disk.find("deleted") == std::string::npos);
    JsonDocument loaded;
    assert(!deserializeJson(loaded, disk));
    std::vector<Entity> rebooted;
    deserializeVector(loaded.as<JsonArray>(), rebooted);
    assert(rebooted.size() == 3);
    assert(rebooted[2].discovery.uniqueId == "add");
    apply(R"([
      {"discovery":{"unique_id":"keep"},"deleted":true},
      {"discovery":{"unique_id":"edit"},"deleted":true},
      {"discovery":{"unique_id":"add"},"deleted":true}
    ])", rebooted);
    assert(rebooted.empty());
    apply(R"([{"discovery":{"unique_id":"unknown"},"deleted":true}])", rebooted);
    assert(rebooted.empty());
}
''')
            executable = temp / "register-patches.exe"
            compiled = subprocess.run([
                COMPILER, "-std=c++23", "-I" + str(temp),
                "-I" + str(ROOT / "firmware/src"),
                "-I" + str(JSON_HEADERS.parent), str(source), "-o", str(executable),
            ], capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            subprocess.run([str(executable)], check=True, capture_output=True, text=True)


if __name__ == "__main__":
    unittest.main()
