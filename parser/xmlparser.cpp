#include "xmlparser.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QXmlStreamReader>


bool XmlParser::loadFromFile(const QString& path, QAbstractItemModel* model)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << Q_FUNC_INFO << "cannot open file:" << path;
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(&file, true)) {
        file.close();
        qWarning() << Q_FUNC_INFO << "invalid XML content";
        return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (!isWorkbook(root)) return false;

    QDomElement worksheet = findWorksheet(root);
    if (worksheet.isNull()) return false;

    QVector<QDomElement> rows = findRows(worksheet);
    if (rows.isEmpty()) return false;

    auto tableModel = qobject_cast<TableModel*>(model);
    if (!tableModel) return false;

    int colCount = determineColumnCount(rows.first());
    model->insertColumns(0, colCount);

    loadHeadersAndTypes(tableModel, rows.first(), colCount);
    loadData(tableModel, rows.mid(1), colCount);

    return true;
}

bool XmlParser::isWorkbook(const QDomElement& element)
{
    return element.localName() == "Workbook" &&
           element.namespaceURI() == "urn:schemas-microsoft-com:office:spreadsheet";
}

QDomElement XmlParser::findWorksheet(const QDomElement& root)
{
    QDomNode child = root.firstChild();
    while (!child.isNull()) {
        if (child.isElement()) {
            QDomElement elem = child.toElement();
            if (elem.localName() == "Worksheet" &&
                elem.namespaceURI() == "urn:schemas-microsoft-com:office:spreadsheet") {
                return elem;
            }
        }
        child = child.nextSibling();
    }
    return QDomElement();
}

QVector<QDomElement> XmlParser::findRows(const QDomElement& worksheet)
{
    QVector<QDomElement> rows;

    QDomNode tableNode = worksheet.firstChild();
    while (!tableNode.isNull()) {
        if (tableNode.isElement() && tableNode.localName() == "Table") {
            QDomNode rowNode = tableNode.firstChild();
            while (!rowNode.isNull()) {
                if (rowNode.isElement() && rowNode.localName() == "Row") {
                    rows.append(rowNode.toElement());
                }
                rowNode = rowNode.nextSibling();
            }
            break;
        }
        tableNode = tableNode.nextSibling();
    }

    return rows;
}

int XmlParser::determineColumnCount(const QDomElement& headerRow)
{
    int colCount = 0, lastIndex = 0;
    QDomNodeList cells = headerRow.elementsByTagName("Cell");

    for (int i = 0; i < cells.count(); ++i) {
        QDomElement cell = cells.at(i).toElement();
        QString indexStr = cell.attributeNS("urn:schemas-microsoft-com:office:spreadsheet", "Index");
        int index = indexStr.isEmpty() ? (lastIndex + 1) : indexStr.toInt();
        lastIndex = index;
        colCount = qMax(colCount, index);
    }

    return colCount;
}

void XmlParser::loadHeadersAndTypes(TableModel* model, const QDomElement& headerRow, int colCount)
{
    int lastIndex = 0;
    QDomNodeList cells = headerRow.elementsByTagName("Cell");

    for (int i = 0; i < cells.count(); ++i) {
        QDomElement cell = cells.at(i).toElement();
        QString indexStr = cell.attributeNS("urn:schemas-microsoft-com:office:spreadsheet", "Index");
        int colIndex = indexStr.isEmpty() ? (lastIndex + 1) : indexStr.toInt();
        lastIndex = colIndex;

        QDomElement dataElem = cell.firstChildElement("Data");
        if (dataElem.isNull()) continue;

        QString type = dataElem.attributeNS("urn:schemas-microsoft-com:office:spreadsheet", "Type");
        QString header = dataElem.text();

        model->setHeaderData(colIndex - 1, Qt::Horizontal, header, Qt::DisplayRole);
    }
}

void XmlParser::loadData(TableModel* model, const QVector<QDomElement>& rows, int colCount)
{
    if (rows.isEmpty()) return;

    determineColumnTypes(model, rows.first());

    for (const QDomElement& rowElem : rows) {
        QVector<QVariant> rowData = parseRow(rowElem, model);
        appendRowToModel(model, rowData);
    }
}

void XmlParser::determineColumnTypes(TableModel* model, const QDomElement& headerRow)
{
    QDomNodeList cells = headerRow.elementsByTagName("Cell");
    for (int j = 0; j < cells.count(); ++j) {
        QDomElement cell = cells.at(j).toElement();
        QDomElement dataElem = cell.firstChildElement("Data");
        if (dataElem.isNull()) continue;

        QString type = dataElem.attributeNS("urn:schemas-microsoft-com:office:spreadsheet", "Type");
        auto columnType = ColumnTypeUtils::fromString(type);
        model->setColumnType(j, columnType);
    }
}

QVector<QVariant> XmlParser::parseRow(const QDomElement& rowElem, TableModel* model)
{
    QVector<QVariant> rowData(model->columnCount());
    int lastIndex = 0;

    QDomNodeList cells = rowElem.elementsByTagName("Cell");

    for (int j = 0; j < cells.count(); ++j) {
        QDomElement cell = cells.at(j).toElement();

        QString indexStr = cell.attributeNS("urn:schemas-microsoft-com:office:spreadsheet", "Index");
        int colIndex = indexStr.isEmpty() ? (lastIndex + 1) : indexStr.toInt();
        lastIndex = colIndex;

        QDomElement dataElem = cell.firstChildElement("Data");
        if (dataElem.isNull()) continue;

        QString text = dataElem.text();
        int col = colIndex - 1;

        if (col >= model->columnCount()) continue;

        switch (model->columnType(col)) {
        case ColumnTypeUtils::BooleanType:
            rowData[col] = (text.toInt() != 0);
            break;
        case ColumnTypeUtils::DateType: {
            QString dtStr = text.split('.').first();
            QDateTime dt = QDateTime::fromString(dtStr, Qt::ISODate);
            rowData[col] = dt.date();
            break;
        }
        case ColumnTypeUtils::NumberType:
            rowData[col] = text.toDouble();
            break;
        default:
            rowData[col] = text;
            break;
        }
    }

    return rowData;
}

void XmlParser::appendRowToModel(TableModel* model, const QVector<QVariant>& rowData)
{
    int row = model->rowCount();
    model->insertRow(row);

    for (int col = 0; col < rowData.size(); ++col) {
        if (rowData[col].isValid())
            model->setData(model->index(row, col), rowData[col]);
    }
}
