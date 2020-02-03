#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QDialog>
#include <QSql>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class ConnectionWindow;
}

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();

private slots:
    void on_connectBtn_clicked();

private:
    Ui::ConnectionWindow *ui;
    QSqlDatabase *db;
    QSqlQueryModel *qm;
    QSqlQuery *q;
    QMessageBox *mb;
    void tryToConnect();
};

#endif // CONNECTIONWINDOW_H
