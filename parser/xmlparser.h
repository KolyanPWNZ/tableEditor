#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QString>
#include <QAbstractItemModel>
#include <QDomDocument>
#include "../tablemodel.h"


class XmlParser
{
public:
    static bool loadFromFile(const QString &path, QAbstractItemModel *model);

private:
    static bool isWorkbook(const QDomElement& element);
    static QDomElement findWorksheet(const QDomElement& root);
    static QVector<QDomElement> findRows(const QDomElement& worksheet);
    static int determineColumnCount(const QDomElement& headerRow);
    static void loadHeadersAndTypes(TableModel* model, const QDomElement& headerRow, int colCount);
    static void loadData(TableModel* model, const QVector<QDomElement>& rows, int colCount);
    static void determineColumnTypes(TableModel* model, const QDomElement& headerRow);
    static QVector<QVariant> parseRow(const QDomElement& rowElem, TableModel* model);
    static void appendRowToModel(TableModel* model, const QVector<QVariant>& rowData);
};

#endif // XMLPARSER_H
