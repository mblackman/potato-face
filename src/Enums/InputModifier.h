#pragma once

enum InputModifier : unsigned int {
    Mod_None = 0x0000,
    Mod_LeftShift = 0x0001,
    Mod_RightShift = 0x0002,
    Mod_LeftControl = 0x0004,
    Mod_RightControl = 0x0008,
    Mod_LeftAlt = 0x0010,
    Mod_RightAlt = 0x0020,

    Mod_Shift = Mod_LeftShift | Mod_RightShift,
    Mod_Control = Mod_LeftControl | Mod_RightControl,
    Mod_Alt = Mod_LeftAlt | Mod_RightAlt
};