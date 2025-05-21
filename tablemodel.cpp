#include "tablemodel.h"

#include <QDate>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return m_columnTypes.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return m_data[index.row()][index.column()];

    return {};
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) return false;

    auto colType = m_columnTypes[index.column()];
    switch (colType) {
    case ColumnTypeUtils::StringType:
        m_data[index.row()][index.column()] = value.toString();
        break;
    case ColumnTypeUtils::NumberType:
        m_data[index.row()][index.column()] = value.toDouble();
        break;
    case ColumnTypeUtils::DateType:
        m_data[index.row()][index.column()] = value.toDate();
        break;
    case ColumnTypeUtils::BooleanType:
        m_data[index.row()][index.column()] = value.toBool();
        break;
    }
    emit dataChanged(index, index, {role});
    return true;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();
    if (section < 0 || section >= m_headers.size())
        return QVariant();
    return m_headers[section];
}

bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{    
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return false;
    if (section >= m_headers.size())
        m_headers.resize(section + 1);
    m_headers[section] = value.toString();
    emit headerDataChanged(orientation, section, section);
    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool TableModel::insertRow(int row, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount())
        return false;

    beginInsertRows(parent, row, row);
    QVector<QVariant> newRow;
    for (int i = 0; i < columnCount(); ++i) {
        newRow.append(QVariant());
    }
    m_data.insert(row, newRow);
    endInsertRows();
    return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        Row rowNew(columnCount());
        m_data.insert(row, rowNew);
    }
    endInsertColumns();
    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_data.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool TableModel::insertColumns(int col, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, col, col + count - 1);
    for (auto& row : m_data) {
        for (int i = 0; i < count; ++i) {
            row.insert(col, QString(""));
        }
    }
    m_columnTypes.insert(col, count, ColumnTypeUtils::StringType);
    m_headers.insert(col, count, QString());
    endInsertColumns();
    return true;
}

bool TableModel::removeColumns(int col, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, col, col + count - 1);
    for (auto& row : m_data) {
        for (int i = 0; i < count; ++i) {
            row.remove(col);
        }
    }
    m_columnTypes.remove(col, count);
    m_headers.remove(col, count);
    endRemoveColumns();
    return true;
}

void TableModel::setColumnType(int col, ColumnTypeUtils::ColumnType type)
{
    m_columnTypes[col] = type;
    emit headerDataChanged(Qt::Horizontal, col, col);
}

ColumnTypeUtils::ColumnType TableModel::columnType(int col) const
{
    if (col < m_columnTypes.size())
        return m_columnTypes[col];
    return ColumnTypeUtils::StringType;
}

const QVector<TableModel::Row> &TableModel::getData() const
{
    return m_data;
}

const QVector<QString> &TableModel::getHeaders() const
{
    return m_headers;
}
