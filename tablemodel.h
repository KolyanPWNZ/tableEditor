#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVector>
#include <QVariant>
#include "columnTypeUtils.h"


class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    int columnCount(const QModelIndex &parent=QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role=Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRow(int row, const QModelIndex &parent=QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;
    bool insertColumns(int col, int count, const QModelIndex &parent=QModelIndex()) override;
    bool removeColumns(int col, int count, const QModelIndex &parent=QModelIndex()) override;

    using Row = QVector<QVariant>;

    void setColumnType(int col , ColumnTypeUtils::ColumnType type);
    ColumnTypeUtils::ColumnType columnType(int col) const;

    const QVector<Row>& getData() const;
    const QVector<QString>& getHeaders() const;

private:
    QVector<Row> m_data;
    QVector<QString> m_headers;
    QVector<ColumnTypeUtils::ColumnType> m_columnTypes;
};

#endif // TABLEMODEL_H
