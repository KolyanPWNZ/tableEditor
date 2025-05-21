#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "delegates/numberdelegate.h"
#include "delegates/datedelegate.h"
#include "delegates/booldelegate.h"
#include "delegates/stringdelegate.h"
#include "serializer/xmlserializer.h"
#include "parser/xmlparser.h"
#include <QFileDialog>
#include <QSharedPointer>
#include <QInputDialog>
#include <QDate>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setModel(QSharedPointer<TableModel>::create(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setModel(QSharedPointer<TableModel> model)
{
    m_model = model;
    m_sortModel = QSharedPointer<NaturalSort>::create(this);
    m_sortModel->setSourceModel(m_model.data());
    ui->tableView->setModel(m_sortModel.data());
    ui->tableView->setSortingEnabled(true);

    updateDelegates();
}

void MainWindow::on_pbLoadFile_clicked()
{
    QString pathFile = QFileDialog::getOpenFileName(this, "Open XML file", "", "XML files (*.xml *.XML *.xmlss *.XMLSS)");
    if (pathFile.isEmpty()) return;

    auto modelNew =  QSharedPointer<TableModel>::create(this);
    if (!XmlParser::loadFromFile(pathFile, modelNew.data()))
        return;
    setModel(modelNew);
}

void MainWindow::on_pbSaveFile_clicked()
{
    QString pathFile = QFileDialog::getSaveFileName(this, "Save as XML", "", "XML files (*.xml *.xmlss)");
    if (!pathFile.isEmpty())
        XmlSerializer::saveToFile(pathFile, m_model.data());
}

void MainWindow::updateDelegates()
{
    for (int col = 0; col < m_model->columnCount(); ++col)
        ui->tableView->setItemDelegateForColumn(col, nullptr);

    if (m_model) {
        for (int col = 0; col < m_model->columnCount(); ++col) {
            switch (m_model->columnType(col)) {
            case ColumnTypeUtils::BooleanType:
                ui->tableView->setItemDelegateForColumn(col, new BoolDelegate(this));
                break;
            case ColumnTypeUtils::DateType:
                ui->tableView->setItemDelegateForColumn(col, new DateDelegate(this));
                break;
            case ColumnTypeUtils::NumberType:
                ui->tableView->setItemDelegateForColumn(col, new NumberDelegate(this));
                break;
            case ColumnTypeUtils::StringType:
                ui->tableView->setItemDelegateForColumn(col, new StringDelegate(this));
                break;
            default:
                break;
            }
        }
    }
}

void MainWindow::on_pbAddColumn_clicked()
{
    bool ok = false;
    QString columnName = QInputDialog::getText(this, "Column Name", "Write column name:", QLineEdit::Normal, "", &ok);
    if (!ok || columnName.isEmpty())
        return;

    ColumnTypeUtils::ColumnType type = askColumnType();
    if (type == ColumnTypeUtils::InvalidType)
        return;

    int col = m_model->columnCount();
    m_model->insertColumn(col);
    m_model->setHeaderData(col, Qt::Horizontal, columnName, Qt::DisplayRole);
    m_model->setColumnType(col, type);
    updateDelegates();
}

ColumnTypeUtils::ColumnType MainWindow::askColumnType()
{
    QStringList items = {"String", "Number", "DateTime", "Boolean"};
    bool ok = false;
    QString item = QInputDialog::getItem(this, "Тип столбца", "Выберите тип:", items, 0, false, &ok);
    if (!ok) return ColumnTypeUtils::InvalidType;

    return ColumnTypeUtils::fromString(item);
}

void MainWindow::on_pbAddRow_clicked()
{
    if (!m_model) return;
    int row = m_model->rowCount();
    m_model->insertRow(row);

    for (int col = 0; col < m_model->columnCount(); ++col) {
        QVariant defaultValue;

        switch (m_model->columnType(col)) {
        case ColumnTypeUtils::StringType:
            defaultValue = "";
            break;
        case ColumnTypeUtils::NumberType:
            defaultValue = 0.0;
            break;
        case ColumnTypeUtils::BooleanType:
            defaultValue = false;
            break;
        case ColumnTypeUtils::DateType:
            defaultValue = QDate::currentDate();
            break;
        }
        m_model->setData(m_model->index(row, col), defaultValue);
    }
}

void MainWindow::on_pbDeleteRow_clicked()
{
    if (!m_model) return;

    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    std::sort(selected.begin(), selected.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.row() > b.row();
    });

    for (const QModelIndex &index : selected) {
        int sourceRow = m_sortModel->mapToSource(index).row();
        m_model->removeRow(sourceRow);
    }
}

void MainWindow::on_pbDeleteColumn_clicked()
{
    if (!m_model) return;

    QModelIndexList selected = ui->tableView->selectionModel()->selectedColumns();
    if (selected.isEmpty()) return;

    std::sort(selected.begin(), selected.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.column() > b.column();
    });

    for (const QModelIndex &index : selected) {
        int sourceRow = m_sortModel->mapToSource(index).column();
        m_model->removeColumn(sourceRow);
    }
}


