#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QObject>
#include <QSql>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QFile>
#include <QDate>
#include <QVariantList>
#include <QStringList>
#include <QLineEdit>
#include <QSqlRecord>
#include <QComboBox>
#include <QMenu>
#include <QSqlField>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include "connectionwindow.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
private slots:
    void showTable();
    void slotCustomMenuReauested(QPoint pos);
    void addNote();
    void delNote();
    void saveChanges();
    void on_saveBtn_clicked();

private:
    Ui::Dialog *ui;
    QSqlTableModel * tm;
    QSqlRelationalTableModel * model;
    QSqlQueryModel *qm;
    QSqlQuery *q;
    QMessageBox *mb;
    QSqlDatabase db;
    ConnectionWindow *cw;
    QStringList tables, fields, data;
    QString tableName;
    QSqlRecord rec;
    QMenu *menu;
    void openConnectWindow();
    bool getTables(/*QVariantList &data*/);
    void setupModel(QString &tableName);
    void setupMainModel(const QString &tableName);
    void createUI();
};

#endif // DIALOG_H
