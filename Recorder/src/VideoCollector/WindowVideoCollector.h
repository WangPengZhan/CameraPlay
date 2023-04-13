#pragma once

#include <Windows.h>

#include "VideoCollector.h"

class WindowVideoCollector : public VideoCollector
{
public:
    WindowVideoCollector();
    ~WindowVideoCollector();

    // ���ò�׽���ڵ�id
    void SetWndHandle(HANDLE hWndCapture);
    HANDLE GetWndHandle() const;

    // תͼ����
    void Captured() override;

private:
    HANDLE m_hWndCapture;
};
