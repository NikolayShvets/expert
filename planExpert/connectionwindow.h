#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

namespace Ui {
class ConnectionWindow;
}

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();
    QSqlDatabase db;
    bool connectFlag{false};

private slots:
    void on_connectBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::ConnectionWindow *ui;
    QMessageBox *mb;
    void tryToConnect();
};

#endif // CONNECTIONWINDOW_H
