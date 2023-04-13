#pragma once

#include "RecorderPort.h"

class RECORDAPI_PORT VideoCollector
{
public:
    VideoCollector();
    ~VideoCollector();

    virtual void Captured() = 0;
};

