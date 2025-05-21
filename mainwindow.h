#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tablemodel.h"
#include "NaturelSort.h"
#include <QAbstractItemDelegate>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbLoadFile_clicked();
    void on_pbSaveFile_clicked();

    void on_pbAddRow_clicked();
    void on_pbAddColumn_clicked();
    void on_pbDeleteRow_clicked();
    void on_pbDeleteColumn_clicked();

private:
    void updateDelegates();
    void setModel(QSharedPointer<TableModel> model);
    ColumnTypeUtils::ColumnType askColumnType();

private:
    Ui::MainWindow *ui;
    QSharedPointer<TableModel> m_model;
    QSharedPointer<NaturalSort> m_sortModel;
    QMap<int, QAbstractItemDelegate*> m_delegates;
};

#endif // MAINWINDOW_H
