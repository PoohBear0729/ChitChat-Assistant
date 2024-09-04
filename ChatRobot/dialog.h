#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QThread>
#include <QMessageBox>
#include <QTimer>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;
private:
    QPushButton * LoginButton;
    QLineEdit * username;
    QLineEdit * Passwd;
    QThread * DisplayWarning;
    QLabel * WarningLabel ;
    QLabel * u;
    QLabel * p;
    char User[11]="1104525078";
    char Passward[7] = "123456";
    void displayWarning();
private slots:
    void Login();
};
class display : public QThread{
    Q_OBJECT
protected:
};
#endif // DIALOG_H
