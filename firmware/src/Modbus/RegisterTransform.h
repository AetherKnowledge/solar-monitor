#include "Common/Enum.h"

enum class RegisterTransform : uint8_t
{
    None,
    Add,
    Subtract,
    Multiply,
    Divide
};

template <>
inline const char *Enum::toString(RegisterTransform value)
{
    switch (value)
    {
    case RegisterTransform::None:
        return "None";
    case RegisterTransform::Add:
        return "Add";
    case RegisterTransform::Subtract:
        return "Subtract";
    case RegisterTransform::Multiply:
        return "Multiply";
    case RegisterTransform::Divide:
        return "Divide";
    }

    return "None";
}

template <>
inline RegisterTransform Enum::fromString<RegisterTransform>(const char *value)
{
    if (strcmp(value, "None") == 0)
        return RegisterTransform::None;
    if (strcmp(value, "Add") == 0)
        return RegisterTransform::Add;
    if (strcmp(value, "Subtract") == 0)
        return RegisterTransform::Subtract;
    if (strcmp(value, "Multiply") == 0)
        return RegisterTransform::Multiply;
    if (strcmp(value, "Divide") == 0)
        return RegisterTransform::Divide;

    return RegisterTransform::None;
}