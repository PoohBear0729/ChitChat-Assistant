#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H
#include <QThread>
#include <QByteArray>
#include <QTcpSocket>
#include <QBuffer>
#include <QAudioFormat>
#include <QAudioSink>
#include <QFile>
#include <QMediaDevices>
#include <QMessageBox>
#include <QSharedPointer>
class AudioThread : public QThread
{
    Q_OBJECT
signals:
    void AudioDataReady(QByteArray data);
public:
    QHostAddress addr;
    QSharedPointer<QTcpSocket> audioClient;
    bool init;

    explicit AudioThread(QObject*parent,QHostAddress addr= QHostAddress("192.168.3.6"));
    ~AudioThread();
protected:
    void run() override;
public slots:
};
#endif // AUDIOTHREAD_H
