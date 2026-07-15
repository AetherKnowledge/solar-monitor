#include "Common.h"

const char *StatusToString(Status status)
{
    switch (status)
    {
    case NotStarted:
        return "not_started";
    case InProgress:
        return "in_progress";
    case UpdateComplete:
        return "complete";
    case UpdateFailed:
        return "failed";
    }

    return "unknown";
}