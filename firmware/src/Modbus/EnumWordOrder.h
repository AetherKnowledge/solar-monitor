#include "Common/Enum.h"
#include "Modbus/ModbusTypes.h"

template <>
inline const char *Enum::toString(WordOrder value)
{
    switch (value)
    {
    case WordOrder::ABCD:
        return "ABCD";
    case WordOrder::BADC:
        return "BADC";
    case WordOrder::CDAB:
        return "CDAB";
    case WordOrder::DCBA:
        return "DCBA";
    }

    return "ABCD";
}

template <>
inline WordOrder Enum::fromString<WordOrder>(const char *value)
{
    if (strcmp(value, "ABCD") == 0)
        return WordOrder::ABCD;
    if (strcmp(value, "BADC") == 0)
        return WordOrder::BADC;
    if (strcmp(value, "CDAB") == 0)
        return WordOrder::CDAB;
    if (strcmp(value, "DCBA") == 0)
        return WordOrder::DCBA;

    return WordOrder::ABCD;
}