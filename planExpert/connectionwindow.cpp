#include "connectionwindow.h"
#include "ui_connectionwindow.h"

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);
    mb = new QMessageBox(this);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::tryToConnect()
{
    db = new QSqlDatabase;
    db->addDatabase("QPSQL");
    db->setHostName("Localhost");
    db->setDatabaseName("ExpertDB");
    db->setUserName(ui->loginEdit->text());
    db->setPassword(ui->passEdit->text());
    if(!db->open()){
        mb->critical(this, QObject::tr("Ошибка подключения к базе данных!"),db->lastError().text());
        qDebug() << db->driverName()<< db->tables();
    }else{
        mb->about(this,QObject::tr("Успешное подключение!"), "Вы вошли под логином " + ui->loginEdit->text());
        q = new QSqlQuery;
        qDebug() << db->driverName()<< db->tables();
        hide();
    }
}

void ConnectionWindow::on_connectBtn_clicked()
{
    tryToConnect();
}
