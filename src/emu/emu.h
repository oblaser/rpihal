/*
author          Oliver Blaser
date            23.03.2024
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#ifndef IG_RPIHAL_EMU_H
#define IG_RPIHAL_EMU_H

#include "olcPixelGameEngine.h"

class EmuPge final : public olc::PixelGameEngine
{
public:
    static const olc::vi2d pgeWindowSize;
    static constexpr int32_t pgePixelSize = 1;

public:
    EmuPge();
    virtual ~EmuPge() {}

public:
    bool OnUserCreate() override;
    bool OnUserUpdate(float tElapsed) override;
    bool OnUserDestroy() override;

private:
    EmuPge(const EmuPge& other) = delete;
    EmuPge(const EmuPge&& other) = delete;
    const EmuPge& operator=(const EmuPge& b) { return b; }
};

#endif // IG_RPIHAL_EMU_H
