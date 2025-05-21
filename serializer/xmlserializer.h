#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include <QString>
#include <QAbstractItemModel>
#include <QXmlStreamWriter>
#include "../tablemodel.h"


class XmlSerializer
{
public:
    static void saveToFile(const QString &path, QAbstractItemModel *model);

private:
    static void writeWorkbookStart(QXmlStreamWriter& writer);
    static void writeDocumentProperties(QXmlStreamWriter& writer);
    static void writeOfficeDocumentSettings(QXmlStreamWriter& writer);
    static void writeExcelWorkbook(QXmlStreamWriter& writer);
    static void writeStyles(QXmlStreamWriter& writer);
    static void writeWorksheet(QXmlStreamWriter& writer, TableModel* model);
    static void writeHeaderRow(QXmlStreamWriter& writer, TableModel* model);
    static void writeDataRows(QXmlStreamWriter& writer, TableModel* model);
    static void writeWorksheetOptions(QXmlStreamWriter& writer);
};

#endif // XMLSERIALIZER_H
