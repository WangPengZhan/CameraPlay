#pragma once

#include <Windows.h>

#include "VideoCollector.h"

class WindowVideoCollector : public VideoCollector
{
public:
    WindowVideoCollector();
    ~WindowVideoCollector();

    // 设置捕捉窗口的id
    void SetWndHandle(HANDLE hWndCapture);
    HANDLE GetWndHandle() const;

    // 转图函数
    void Captured() override;

private:
    HANDLE m_hWndCapture;
};
