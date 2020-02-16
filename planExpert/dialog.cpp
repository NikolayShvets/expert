#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Редактор базы данных");
    menu = new QMenu(this);
    /* создаем объект контекстного меню*/
    /* добавляем действия */
    QAction *addanote = new QAction(trUtf8("Добавить запсиь"), this);
    QAction *delanote = new QAction(trUtf8("Удалить запись"), this);
    QAction *alteration = new QAction(trUtf8("Редактировать запись"), this);
    /* Подключаем слоты обработки действий контекстного меню */
    connect(addanote, SIGNAL(triggered()), this, SLOT(addNote()));
    connect(delanote, SIGNAL(triggered()), this, SLOT(delNote()));
    /* утсновка действий в меню */
    menu->addAction(addanote);
    menu->addAction(delanote);
    menu->addAction(alteration);
    openConnectWindow();
}

void Dialog::openConnectWindow()
{
    cw = new ConnectionWindow(this);
    cw->setModal(true);
    cw->exec();
    if(cw->connectFlag){
        db=cw->db;
        qDebug() <<"test"<< this->db.driverName()<< db.tables();
        getTables();
    }else{
        this->ui->saveBtn->setEnabled(false);
    }
}

bool Dialog::getTables(/*QVariantList &data*/)
{
    q = new QSqlQuery();
    tables.append(db.tables());
    this->ui->selectTableComboBox->setEditable(true);
    this->ui->selectTableComboBox->lineEdit()->setText("Выберите таблицу...");

    for(int i = 0; i < tables.count(); ++i){
        q->exec("SELECT *FROM " + tables[i]);
        rec = q->record();
        for(int j = 0; j < rec.count(); j++){
            fields.append(rec.fieldName(j));
             //qDebug()<<"filed name:"<<rec.fieldName(j)<<
               //             "type"<<rec.fieldName(j).ty << "relation: " << rec.keyValues(rec);
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

void Dialog::setupMainModel(const QString &tableName)
{
    model = new QSqlRelationalTableModel(this);
    model->setTable(tableName);
   // model->setRelation(1, QSqlRelation("targettype", "id", "name"));
    model->setSort(0, Qt::AscendingOrder);
    model->select();
}

void Dialog::createUI()
{
    /*this->ui->tablesTableView->setModel(tm);
    this->ui->tablesTableView->setItemDelegate(new QSqlRelationalDelegate(tableData));
 // tableData->setItemDelegateForRow(1, new QSqlRelationalDelegate(tableData));
 // tableData->setItemDelegateForRow(2, new QSqlRelationalDelegate(tableData));
    this->ui->tablesTableView->setColumnHidden(tm->columnCount() - 1,true);
 // Разрешаем выделение строк
    this->ui->tablesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
 // Устанавливаем режим выделения лишь одно строки в таблице
    this->ui->tablesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //tableData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tablesTableView->horizontalHeader()->setStretchLastSection(true);
    this->ui->tablesTableView->setContextMenuPolicy(Qt::CustomContextMenu);*/

    this->ui->tablesTableView->setModel(model);
    this->ui->tablesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->tablesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
   // this->ui->tablesTableView->setItemDelegate(new QSqlRelationalDelegate(this->ui->tablesTableView));
   // this->ui->tablesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tablesTableView->setColumnHidden(0,true);
    this->ui->tablesTableView->horizontalHeader()->setStretchLastSection(true);
    this->ui->tablesTableView->setContextMenuPolicy(Qt::CustomContextMenu);

  //  connect(this->ui->tablesTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(addNote()));
    connect(this->ui->tablesTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuReauested(QPoint)));

    model->select();
    this->ui->tablesTableView->resizeColumnsToContents();
    //tm->select();
 // Устанавливаем размер колонок по содержимому
    //this->ui->tablesTableView->resizeColumnsToContents();
}

void Dialog::showTable()
{
    tableName = this->ui->selectTableComboBox->currentText();
    qDebug()<<"selected table: "<<tableName;
    //this->setupModel(tableName);
    this->setupMainModel(tableName);
    this->createUI();

}

void Dialog::addNote()
{
    int row = this->ui->tablesTableView->selectionModel()->currentIndex().row() + 1;
    model->insertRow(row);
    /*q->prepare("INSERT INTO criticalcombination (start_x) values (1000)");
    q->exec();
    qDebug()<<db.lastError();
    q->prepare("SELECT column_name FROM information_schema.columns WHERE table_name =  'target' ");
    q->exec();
    QString name = q->value(0).toString();
    /*while (q->next()) {
             QString name = q->value(0).toString();
             qDebug() << name;
         }*/
    /*qDebug()<<name;
    qDebug()<<db.lastError();*/
}

void Dialog::delNote()
{
    /* получаем индекс выбранной строки */
    int row = this->ui->tablesTableView->selectionModel()->currentIndex().row();
    qDebug()<<row;

    /* проверяем была ли строка действительно выбрана */
    if (row >= 0){
        /* спрашиваем удалить запись или нет */
        if(QMessageBox::warning(this, trUtf8("Удаление записи"), trUtf8("Удалить запись?"),
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            /* При отрицательном ответе делаем откат действий
             * и закрываем диалог без удаления записи
             * */
            db.rollback();
            qDebug()<<"NO";
            return;
        }else{
            /* Иначе удаляем запись
             * при успешном удалении обновляем таблицу
             * */
            this->ui->tablesTableView->model()->removeRow(row);
            qDebug()<<"YES";
            if(!model->removeRow(row)){
                QMessageBox::warning(this, trUtf8("Уведомление"), trUtf8("Не удалось удалить запись\n"
                                                                             "Возможно она используется другими таблицами\n"                                                             "Проверьте зависимости и повторите попытку"));
            }
            model->select();
            this->ui->tablesTableView->setCurrentIndex(model->index(-1,-1));
        }
    }
}

void Dialog::slotCustomMenuReauested(QPoint pos)
{
    this->ui->tablesTableView->viewport()->update();
    menu->popup(this->ui->tablesTableView->viewport()->mapToGlobal(pos));
}

void Dialog::saveChanges()
{
    model->submit();
    if(!model->submitAll()){
        qDebug()<<"False: "<<model->lastError();
        mb->critical(this, QObject::tr("Ошибка обращения к базе данных!"),db.lastError().text());
    }else{
        qDebug()<<"true"<<model->lastError();
    }
    model->select();
    this->ui->tablesTableView->resizeColumnsToContents();
}

Dialog::~Dialog()
{
    this->db.close();
    delete ui;
}

void Dialog::on_saveBtn_clicked()
{
    saveChanges();
}
