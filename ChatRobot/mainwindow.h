#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QScrollBar>
#include <QTreeView>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QMenu>
#include <QSocketDescriptor>
#include <QTcpSocket>
#include <QInputDialog>
#include <QAbstractSocket>
#include <QString>
#include <QThread>
#include "AudioThread.h"
#include <QTimer>
#include <QSoundEffect>
class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void AudioDataReady(QByteArray &data);
    void AudioPlayEnd();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getData(QTcpSocket * audioClient);
private:
    QPlainTextEdit * dialogBox;
    QLineEdit * SendBox;
    QPushButton * ConfirmButton;
    QPushButton * NewDialogButton;
    QScrollBar * TextBar;
    QTreeView * ChoseHistory;
    QString CurrentDialog;
    QStandardItemModel * model;
    QStandardItem  itemList[100];
    QList<QStandardItem*>ItemList;
    QTcpSocket * client;
    QTcpSocket * audio_client;
    AudioThread * audioThread;
    //QSoundEffect * AudioOutput;
    QAudioSink * AudioOutput;
    QBuffer  buff;
    bool validData;
    int ItemSize;
    void InitQTreeView();
    QHostAddress connectServer();


protected:
    void paintEvent(QPaintEvent *event);
    void flushPlainEdit();


private slots:
    void DeleteBuff();
    void ConfirmInformation();
    void clicked(const QModelIndex &index);
    void slotTreeMenu(const QPoint &pos);
    void deleteHistory(const QPoint &pos);
    void NewDialog();
    void ProcessAudioData(QByteArray data);
};

#endif // MAINWINDOW_H
