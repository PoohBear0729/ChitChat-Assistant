#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(1024, 720, 1024, 720);

    SendBox = new QLineEdit(this);
    ConfirmButton = new QPushButton(this);
    NewDialogButton = new QPushButton(this);
    ChoseHistory = new QTreeView(this);
    dialogBox = new QPlainTextEdit(this);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList("消息历史"));
    dialogBox->setGeometry(0, 0, 633, 445);
    SendBox->setGeometry(110, 550, 500, 30);
    ConfirmButton->setGeometry(650, 550, 45, 45);
    NewDialogButton->setGeometry(700,450,50,40);
    ChoseHistory->setGeometry(640, 0, 382, 445);
    NewDialogButton->setText("创建新对话");
    ConfirmButton->setStyleSheet("QPushButton{background-image: url(/home/alientek/button.png)}");
    dialogBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    dialogBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    dialogBox->setReadOnly(true);  // 保持为只读模式

    CurrentDialog = "NULL";
    ChoseHistory->setModel(model);
    ChoseHistory->expandAll();
    ChoseHistory->setEditTriggers(QTreeView::NoEditTriggers);
    ChoseHistory->setSelectionBehavior(QTreeView::SelectRows);
    ChoseHistory->setContextMenuPolicy(Qt::CustomContextMenu);
    client = new QTcpSocket(this);
    int count = 0;
    QHostAddress serverIP;
    while(client->state()!= QAbstractSocket::ConnectedState){
        serverIP = connectServer();
        if(serverIP.isEqual(QHostAddress("127.0.0.1"))){
            exit(-1);
        }
        client->connectToHost(serverIP, 8888);
        if(!client->waitForConnected(60)){
            QMessageBox::critical(this, "服务器错误", "服务器连接超时，请确认服务器状态或检查IP地址");
            count++;
            continue;
        }
        else if(count==10){
            exit(-1);
        }
    audioThread = new AudioThread(this,serverIP);
    }
    QAudioFormat format;
    format.setSampleRate(32000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    if(!format.isValid()){
        qDebug()<<"do not support this format"<<Qt::endl;
    }
    AudioOutput = new QAudioSink(format,this);
    AudioOutput->setVolume(0.5f);
    InitQTreeView();
    connect(ChoseHistory, &QTreeView::clicked, this, &MainWindow::clicked);
    connect(ConfirmButton, SIGNAL(clicked()), this, SLOT(ConfirmInformation()));
    connect(ChoseHistory, &QTreeView::customContextMenuRequested, this, &MainWindow::slotTreeMenu);
    connect(NewDialogButton, SIGNAL(clicked()), this, SLOT(NewDialog()));
    connect(audioThread, &AudioThread::AudioDataReady, this, &MainWindow::ProcessAudioData);
    connect(AudioOutput, &QAudioSink::stateChanged, this, &MainWindow::DeleteBuff);
}
void MainWindow::DeleteBuff(){
    if(AudioOutput->state() == QAudio::IdleState){
        buff.close();
        buff.setData(QByteArray());
    }
}
void MainWindow::ProcessAudioData(QByteArray data) {
    qDebug()<<"data size"<<data.size()<<Qt::endl;
    if (buff.isOpen()) {
        buff.close();
    }
    buff.setData(data);
    if (!buff.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open QBuffer" << Qt::endl;
        return;
    }
    QFile file("/home/alientek/Chat_Audio/response.wav");
    if(file.open(QFile::ReadWrite)){
        file.write(data);
        file.flush();
    }
    else{
        qDebug()<<file.errorString()<<Qt::endl;
    }
        qDebug()<<"Audio play"<<Qt::endl;
    AudioOutput->start(&buff);
    file.close();
}

void MainWindow::NewDialog() {
    // 生成新文件名
    QString fileName = QString("/home/alientek/Chat_History/%1.txt").arg(ItemSize + 1);

    // 创建并初始化 QStandardItem
    QStandardItem *newItem = new QStandardItem(QString::number(ItemSize + 1));
    ItemList.append(newItem);

    // 打开文件以创建新对话
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return;
    }
    file.close();

    // 清空对话框
    dialogBox->clear();

    // 将新项添加到 QTreeView
    QStandardItem *rootItem = model->invisibleRootItem();
    rootItem->appendRow(newItem);

    // 更新对话列表和对话数
    ItemSize++;
}

QHostAddress MainWindow::connectServer(){
    bool save = false;
    QString addr = QInputDialog::getText(this, "连接服务器", "请输入服务器IP地址", QLineEdit::Normal, "192.168.5.11", &save);
    if(!save){
        addr = "127.0.0.1";
    }
    return QHostAddress(addr);
}
void MainWindow::flushPlainEdit(){
    QString fileName = QString("/home/alientek/Chat_History/%1").arg(CurrentDialog);
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    dialogBox->clear();
    dialogBox->insertPlainText(in.readAll());
    file.close();
}
void MainWindow::deleteHistory(const QPoint &pos) {
    //用数组会发生内存问题
    int index = ChoseHistory->indexAt(pos).row();
    if (index < 0 || index >= ItemSize) {
        qDebug() << "Invalid index:" << index;
        return;
    }
    QString fileName = QString("/home/alientek/Chat_History/%1.txt").arg(index);
    QFile file(fileName);
    // 删除文件
    if (file.exists()) {
        if (!file.remove()) {
            qDebug() << "Failed to delete the file:" << file.errorString();
            return;
        }
    }
    QStandardItem *rootItem = model->invisibleRootItem();
    rootItem->removeRow(index);
    // 重命名后续文件
    for (int i = index; i < ItemSize-1; ++i) {
        QString oldName = QString("/home/alientek/Chat_History/%1.txt").arg(i+1);
        QString newName = QString("/home/alientek/Chat_History/%1.txt").arg(i);
        ItemList.swapItemsAt(i,i+1);
        if (!QFile::rename(oldName, newName)) {
            qDebug() << "Failed to rename the file:" << Qt::endl;
            return;
        }
        // 更新 itemList 中的文本
        ItemList[i]->setText(QString::number(i));
    }
    // 从模型中移除对应行
    // 逻辑删除 itemList 中的项

    ItemList.removeAt(ItemSize-1);
    // 更新项目总数
    ItemSize--;
    flushPlainEdit();
}
void MainWindow::slotTreeMenu(const QPoint &pos){
    QMenu menu;
    QModelIndex index = ChoseHistory->indexAt(pos);
    if(index.isValid()){
        QAction* deleteAction = menu.addAction(QStringLiteral("删除"));
        connect(deleteAction, &QAction::triggered, this, [this, pos]() {
            deleteHistory(pos);  // 使用lambda表达式来捕获位置信息
        });
    }
    menu.exec(QCursor::pos());
}
void MainWindow::paintEvent(QPaintEvent *) {


}
void MainWindow::clicked(const QModelIndex &index) {
    if (index.isValid()) {
        QString data = index.data().toString();
        this->CurrentDialog = QString("%1.txt").arg(data);
        if (CurrentDialog != "NULL") {
            QString FileName = QString("/home/alientek/Chat_History/%1").arg(CurrentDialog);
            QFile File(FileName);
            if (File.open(QFile::ReadOnly)) {
                dialogBox->clear();
                dialogBox->insertPlainText(File.readAll());
            }
        }
    }
}
void MainWindow::InitQTreeView() {
    this->ItemSize = 0;
    QDir storeDir("/home/alientek/Chat_History");

    if (!storeDir.exists()) {
        if (!storeDir.mkpath(".")) {
            qDebug() << "Failed to create directory:" << storeDir.path();
            return;
        }
    }

    QFileInfoList FileList = storeDir.entryInfoList(QDir::Files | QDir::CaseSensitive);
    if (FileList.isEmpty()) {
        return;
    }

    this->ItemSize = FileList.size();

    if (this->ItemSize > 100) {
        qDebug() << "Too many files, only the first 100 will be shown.";
        this->ItemSize = 100;  // 只使用前 100 个项目
    }
    QStandardItem *rootItem = model->invisibleRootItem();
    for (int var = 0; var < this->ItemSize; ++var) {
        QStandardItem * tmp = new QStandardItem(FileList[var].baseName());
        ItemList.append(tmp);
        rootItem->appendRow(ItemList[var]);
    }
}
void MainWindow::ConfirmInformation() {
    QString data = SendBox->text();
    if (data.isEmpty()) {
        qDebug()<<"SendBox is empty"<<Qt::endl;
        return;
    }
    qDebug()<<"data is"<<SendBox->text()<<Qt::endl;
    qDebug()<<"CurrentDialog i"<<this->CurrentDialog<<Qt::endl;
    if (CurrentDialog == "NULL") {
        QDir storeDir("/home/alientek/Chat_History");
        QFileInfoList FileList = storeDir.entryInfoList(QDir::Files | QDir::CaseSensitive);
        if (!storeDir.exists()) {
            if (!storeDir.mkpath(".")) {
                qDebug() << "Failed to create directory:" << storeDir.path();
                return;
            }
        }
        int MaxNum = -1;
        if (!FileList.isEmpty()) {
            MaxNum = FileList[0].baseName().toInt();
            for (int i = 0; i < FileList.size(); ++i) {
                QString suffix = FileList[i].suffix();
                if (suffix.compare("txt") == 0) {
                    QString prefix = FileList[i].baseName();
                    if (prefix.toInt() > MaxNum) MaxNum = prefix.toInt();
                }
            }
        }
        if (MaxNum == 99) {
            QMessageBox::warning(this, "警告", "消息历史已满");
            return;
        }
        QString FileName = QString("/home/alientek/Chat_History/%1.txt").arg(MaxNum + 1);
        QFile file(FileName);
        if (!file.open(QFile::Append|QFile::ReadWrite)) {
            qDebug() << "file cannot be opened" << Qt::endl;
            return;
        }
        CurrentDialog = QString::number(MaxNum + 1) + ".txt";
        QString dialog = data.toUtf8();
        QByteArray byteArray = dialog.toUtf8();
        file.write("用户: "+data.toUtf8()+"\n");
        client->write(CurrentDialog.toUtf8(), CurrentDialog.toUtf8().size());
        std::string flag;
        if(client->waitForReadyRead(60000)){
             flag = client->read(64).toStdString();
        }
        qDebug()<<flag<<Qt::endl;
        if(flag=="ok"){
            qDebug()<<"发送对话"<<Qt::endl;
            client->write(byteArray);
        }
        audioThread->start();
        QString response;
        if(client->waitForReadyRead(120000)){
            QByteArray byteArray = client->readAll();
            response = QString::fromUtf8(byteArray);
        }
        else{
            qDebug()<<"服务器发送数据超时或错误"<<Qt::endl;
            exit(-1);
        }
        file.flush();
        QTextStream out(&file);
        dialogBox->insertPlainText("用户: "+data.toUtf8()+"\n");
        dialogBox->insertPlainText(response+"\n");
        out<<response;
        QStandardItem * tmp = new QStandardItem(QString::number(MaxNum+1));
        ItemList.append(tmp);
        QStandardItem *rootItem = model->invisibleRootItem();
        rootItem->appendRow(ItemList[MaxNum+1]);
        ItemSize+=1;
        if(audioThread->isRunning()){
            qDebug()<<"等待语音线程"<<Qt::endl;
            audioThread->wait(30000);
        }
        file.close();
        SendBox->clear();
    } else {
        QString FileName = QString("/home/alientek/Chat_History/%1").arg(CurrentDialog);
        QFile file(FileName);
        if (!file.open(QFile::ReadWrite|QFile::Append)) {
            qDebug() << "file cannot be opened" << Qt::endl;
            return;
        }
        QTextStream in(&file);
        QString dialog = data.toUtf8();
        QByteArray byteArray = dialog.toUtf8();
        byteArray = "用户: "+byteArray;
        dialogBox->appendPlainText(byteArray);
        client->write(CurrentDialog.toUtf8());
        std::string flag;
        if(client->waitForReadyRead(60000)){
            flag = client->read(64).toStdString();
        }
        qDebug()<<flag<<Qt::endl;
        if(flag=="ok"){
            qDebug()<<"发送对话"<<Qt::endl;
            client->write(byteArray);
        }
        else{
            return ;
        }
        audioThread->start();
        QString response;
        if(client->waitForReadyRead(120000)){

            QByteArray byteArray = client->readAll();
            response = QString::fromUtf8(byteArray);
        }
        else{
            qDebug()<<"服务器发送数据超时或错误"<<Qt::endl;
            exit(-1);
        }
        file.seek(file.size());
        QTextStream out(&file);
        out<<byteArray<<"\n";
        out<<response<<"\n";
        dialogBox->insertPlainText(byteArray+"\n");
        dialogBox->insertPlainText(response+"\n");
        if(audioThread->isRunning()){
            qDebug()<<"等待语音线程"<<Qt::endl;
            audioThread->wait(30000);
        }        else{
            qDebug()<<"语音线程未在运行或结束"<<Qt::endl;
        }
         file.close();
        SendBox->clear();
    }
}
MainWindow::~MainWindow() {
    if (audioThread->isRunning()) {
        audioThread->quit();
        audioThread->wait();
    }
     delete audioThread;
}
