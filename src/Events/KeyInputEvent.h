#pragma once

#include <string>

#include "../Enums/InputKey.h"
#include "../Enums/InputModifier.h"

struct KeyInputEvent : public Event {
   public:
    InputKey inputKey;
    InputModifier inputModifier;
    bool isPressed;
    std::string inputString;

    KeyInputEvent(InputKey inputKey, InputModifier inputModifier, bool isPressed, std::string inputString)
        : inputKey(inputKey),
          inputModifier(inputModifier),
          isPressed(isPressed),
          inputString(inputString) {}
};