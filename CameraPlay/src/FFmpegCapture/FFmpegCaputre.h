#pragma once

#include <QObject>
#include <QString>

#include <thread>
#include <condition_variable>
#include <mutex>

class FFmpegCaputre  : public QObject
{
    Q_OBJECT

public:
    FFmpegCaputre(QObject* parent = nullptr);
    ~FFmpegCaputre();

    void SetFilename(const QString& strDeviceName);
    void Start();
    void Stop();

protected:
    void CaptureRun();
    QString m_strDeviceName;

signals:
    void captureImage(const QImage& image);

private:
    volatile bool m_bStarted;
    volatile bool m_bStop;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::thread m_thread;

};
