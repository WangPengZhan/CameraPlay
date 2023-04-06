#pragma once

class WindowVideoCollector
{
public:
    WindowVideoCollector();
    ~WindowVideoCollector();

    void SetWndHandle(HANDLE hWndCapture);
    HANDLE GetWndHandle() const;

    void Captured();

private:
    HANDLE m_hWndCapture;
};
