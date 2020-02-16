#include "connectionwindow.h"
#include "ui_connectionwindow.h"

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Авторизация");
    mb = new QMessageBox(this);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
    db.close();
}

void ConnectionWindow::tryToConnect()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("Localhost");
    db.setDatabaseName("expertDB");
    db.setUserName(ui->loginEdit->text());
    db.setPassword(ui->passEdit->text());
    if(!db.open()){
        mb->critical(this, QObject::tr("Ошибка подключения к базе данных!"),db.lastError().text());
    }else{
        mb->information(this,QObject::tr("Успешное подключение!"), "Вы вошли под логином " + ui->loginEdit->text());
        qDebug() << db.driverName()<< db.tables();
        connectFlag = true;
        hide();
    }
}


void ConnectionWindow::on_connectBtn_clicked()
{
    tryToConnect();
}

void ConnectionWindow::on_cancelBtn_clicked()
{
    hide();
}
