#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setGeometry(1024,720,480,480);

    // 初始化控件
    LoginButton = new QPushButton(this);
    username = new QLineEdit(this);
    Passwd = new QLineEdit(this);
    WarningLabel = new QLabel(this);

    // 设置控件的几何位置
    username->setGeometry(20,40,200, 30);
    Passwd->setGeometry(20,100,200,30);
    LoginButton->setGeometry(280,40, 40,40);
    LoginButton->setText("Login");

    u = new QLabel(this);
    p = new QLabel(this);
    u->setGeometry(20,20,80,20);
    u->setText("Username");
    p->setGeometry(20,80,80,20);
    p->setText("Password");

    WarningLabel->setGeometry(20,180,280,20);
    WarningLabel->setStyleSheet("color: red;");
    WarningLabel->clear();  // 初始状态不显示警告

    QObject::connect(LoginButton, SIGNAL(clicked()), this, SLOT(Login()));
}

void Dialog::Login(){
    // 清除之前的警告信息
    WarningLabel->clear();

    if(username->text().isEmpty() || Passwd->text().isEmpty()){
        WarningLabel->setText("The username or password can't be empty");

        // 在主线程中设置一个定时器来清除警告信息
        QTimer::singleShot(3000, this, [this]() {
            WarningLabel->clear();
        });
    }
    else {
        if(username->text() == User && Passwd->text() == Passward){
            accept();  // 关闭登录窗口，进入主窗口
        }
        else {
            QMessageBox::critical(this, "Error", "The password or account is incorrect");
        }
    }
}

Dialog::~Dialog()
{
    delete ui;
    delete LoginButton;
    delete username;
    delete Passwd;
    delete u;
    delete p;
    delete WarningLabel;
}
