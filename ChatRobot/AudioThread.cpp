#include "AudioThread.h"

AudioThread::AudioThread(QObject*parent,QHostAddress addr):QThread(parent){
    this->addr = addr;
}
void AudioThread::run() {
    if(!init){
        audioClient = QSharedPointer<QTcpSocket>::create();
        init = true;
    }
    audioClient->connectToHost(addr, 6666);
    if(!audioClient->waitForConnected(35000)){

        exit(-1);
    }
    qDebug() << "语音获取" << Qt::endl;

    // 等待并获取文件大小
    if (!audioClient->waitForReadyRead(40000)) {
        qDebug() << "语音文件大小获取超时";
        return;
    }
    QByteArray fileSizeData = audioClient->read(64);
    qint64 fileSize = fileSizeData.toULongLong();  // 使用适当的方法获取文件大小
    qDebug() << "语音文件大小:" << fileSize << Qt::endl;

    // 发送确认信息
    audioClient->write("ok");

    // 开始接收音频数据
    QByteArray byteArray;
    qint64 totalReceived = 0;

    while (totalReceived < fileSize) {
        if (!audioClient->waitForReadyRead(30000)) {
            qDebug() << "语音数据接收超时" << fileSize << Qt::endl;
            return;
        }

        QByteArray chunk = audioClient->readAll();
        if (chunk.isEmpty()) {
            qDebug() << "连接意外中断";
            return;
        }

        byteArray.append(chunk);
        totalReceived += chunk.size();

        // 发送确认信息
        audioClient->write("ok");
        audioClient->waitForBytesWritten(6000);

    }

    qDebug() << "语音获取成功, 数据大小:" << byteArray.size() << Qt::endl;
    audioClient->close();
    // 发送信号，通知主线程音频数据已准备好
    emit AudioDataReady(byteArray);
}


AudioThread::~AudioThread(){}
