#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    openConnectWindow();
}

void Dialog::openConnectWindow()
{
    cw = new ConnectionWindow(this);
    cw->setModal(true);
    cw->exec();
    db=cw->db;
    qDebug() <<"test"<< this->db.driverName()<< db.tables();
    getTables();
}

bool Dialog::getTables(/*QVariantList &data*/)
{
    q = new QSqlQuery();
    tables.append(db.tables());
    this->ui->selectTableComboBox->setEditable(true);
    this->ui->selectTableComboBox->lineEdit()->setPlaceholderText("Выберите таблицу...");

    for(int i = 0; i < tables.count(); ++i){
        q->exec("SELECT *FROM " + tables[i]);
        rec = q->record();
        for(int j = 0; j < rec.count(); j++){
            fields.append(rec.fieldName(j));
            // qDebug()<<"filed name:"<</*rec.fieldName(j)<<
                     //       "type"<<rec.field(j).type() << */"relation: " << rec.keyValues(rec);
        }
        this->ui->selectTableComboBox->addItem(tables[i]);
    }

    connect(this->ui->selectTableComboBox, SIGNAL(activated(int)), this, SLOT(showTable()));

}

void Dialog::setupModel(QString &tableName)
{
    tm = new QSqlTableModel;
    tm->setTable(tableName);
    for(int i = 0, j = 0; i < tm->columnCount(); i++, j++){
        tm->setSort(0,Qt::AscendingOrder);
    }
}

void Dialog::createUI()
{
    this->ui->tablesTableView->setModel(tm);
    //this->ui->tablesTableView->setItemDelegate(new QSqlRelationalDelegate(tableData));
 // tableData->setItemDelegateForRow(1, new QSqlRelationalDelegate(tableData));
 // tableData->setItemDelegateForRow(2, new QSqlRelationalDelegate(tableData));
    this->ui->tablesTableView->setColumnHidden(0,true);
 // Разрешаем выделение строк
    this->ui->tablesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
 // Устанавливаем режим выделения лишь одно строки в таблице
    this->ui->tablesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //tableData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tablesTableView->horizontalHeader()->setStretchLastSection(true);
    this->ui->tablesTableView->setContextMenuPolicy(Qt::CustomContextMenu);

  //connect(tableData, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(addANote()));
  //connect(tableData, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuReauested(QPoint)));

    tm->select();
 // Устанавливаем размер колонок по содержимому
    this->ui->tablesTableView->resizeColumnsToContents();
}

void Dialog::showTable()
{
    tableName = this->ui->selectTableComboBox->currentText();
    qDebug()<<"selected table: "<<tableName;
    this->setupModel(tableName);
    this->createUI();
}

void Dialog::slotCustomMenuReauested(QPoint pos)
{
    /* создаем объект контекстного меню*/
        menu = new QMenu(this);
        /* добавляем действия */
        QAction *addanote = new QAction(trUtf8("Добавить запсиь"), this);
        QAction *delanote = new QAction(trUtf8("Удалить запись"), this);
        QAction *alteration = new QAction(trUtf8("Редактировать запись"), this);
        /* Подключаем слоты обработки действий контекстного меню */
        connect(addanote, SIGNAL(triggered()), this, SLOT(addANote()));
        connect(delanote, SIGNAL(triggered()), this, SLOT(dellANote()));
        connect(alteration, SIGNAL(triggered()), this, SLOT(showCorrectForm()));
        /* утсновка действий в меню */
        menu->addAction(addanote);
        menu->addAction(delanote);
        menu->addAction(alteration);
        /* вызов контекстного меню */
        //tableData->viewport()->update();
        menu->popup(this->ui->tablesTableView->viewport()->mapToGlobal(pos));
        //menu->close();
}

Dialog::~Dialog()
{
    this->db.close();
    delete ui;
}
