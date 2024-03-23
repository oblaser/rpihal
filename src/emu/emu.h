/*
author          Oliver Blaser
date            23.03.2024
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#ifndef IG_RPIHAL_EMU_H
#define IG_RPIHAL_EMU_H

#include "olcPixelGameEngine.h"

class Emu final : public olc::PixelGameEngine
{
public:
    static const olc::vi2d pgeWindowSize;
    static constexpr int32_t pgePixelSize = 1;

public:
    Emu();
    virtual ~Emu() {}

public:
    bool OnUserCreate() override;
    bool OnUserUpdate(float tElapsed) override;
    bool OnUserDestroy() override;

private:
    Emu(const Emu& other) = delete;
    Emu(const Emu&& other) = delete;
    Emu& operator=(const Emu& b) {}
};

#endif // IG_RPIHAL_EMU_H
