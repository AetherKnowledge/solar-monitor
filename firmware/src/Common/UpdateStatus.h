#pragma once

#include <Common/Enum.h>

enum class UpdateStatus : uint8_t {
    Requested,
    InProgress,
    UpdateComplete,
    NotStarted,
    UpdateFailed
};

template <>
inline const char* Enum::toString(UpdateStatus value) {
    switch (value) {
        case UpdateStatus::Requested:
            return "requested";

        case UpdateStatus::NotStarted:
            return "not_started";

        case UpdateStatus::InProgress:
            return "in_progress";

        case UpdateStatus::UpdateComplete:
            return "complete";

        case UpdateStatus::UpdateFailed:
            return "failed";
    }

    return "unknown";
}

template <>
inline UpdateStatus Enum::fromString<UpdateStatus>(const char* value) {
    if (strcmp(value, "not_started") == 0)
        return UpdateStatus::NotStarted;

    if (strcmp(value, "in_progress") == 0)
        return UpdateStatus::InProgress;

    if (strcmp(value, "complete") == 0)
        return UpdateStatus::UpdateComplete;

    if (strcmp(value, "failed") == 0)
        return UpdateStatus::UpdateFailed;

    return UpdateStatus::NotStarted;
}