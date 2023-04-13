#include "WindowVideoCollector.h"

WindowVideoCollector::WindowVideoCollector()
{
}

WindowVideoCollector::~WindowVideoCollector()
{
}

void WindowVideoCollector::SetWndHandle(HANDLE hWndCapture)
{
    m_hWndCapture = hWndCapture;
}

HANDLE WindowVideoCollector::GetWndHandle() const
{
    return m_hWndCapture;
}

void WindowVideoCollector::Captured()
{
}
