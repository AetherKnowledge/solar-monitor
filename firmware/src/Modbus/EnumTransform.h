#include "Common/Enum.h"

enum class ReadRegisterTransform : uint8_t
{
    None,
    Add,
    Subtract,
    Multiply,
    Divide
};

enum class WordOrder : uint8_t
{
    ABCD,
    BADC,
    CDAB,
    DCBA
};

template <>
inline const char *Enum::toString(ReadRegisterTransform value)
{
    switch (value)
    {
    case ReadRegisterTransform::None:
        return "None";
    case ReadRegisterTransform::Add:
        return "Add";
    case ReadRegisterTransform::Subtract:
        return "Subtract";
    case ReadRegisterTransform::Multiply:
        return "Multiply";
    case ReadRegisterTransform::Divide:
        return "Divide";
    }

    return "None";
}

template <>
inline ReadRegisterTransform Enum::fromString<ReadRegisterTransform>(const char *value)
{
    if (strcmp(value, "None") == 0)
        return ReadRegisterTransform::None;
    if (strcmp(value, "Add") == 0)
        return ReadRegisterTransform::Add;
    if (strcmp(value, "Subtract") == 0)
        return ReadRegisterTransform::Subtract;
    if (strcmp(value, "Multiply") == 0)
        return ReadRegisterTransform::Multiply;
    if (strcmp(value, "Divide") == 0)
        return ReadRegisterTransform::Divide;

    return ReadRegisterTransform::None;
}