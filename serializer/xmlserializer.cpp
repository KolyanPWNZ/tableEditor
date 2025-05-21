#include "xmlserializer.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>


void XmlSerializer::saveToFile(const QString& path, QAbstractItemModel* model)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << Q_FUNC_INFO << "can't open file:" << path;
        return;
    }

    auto tableModel = qobject_cast<TableModel*>(model);
    if (!tableModel) {
        qWarning() << Q_FUNC_INFO << "invalid model cast to TableModel";
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);

    writeWorkbookStart(writer);                  // <ss:Workbook>
    writeDocumentProperties(writer);             // <o:DocumentProperties>
    writeOfficeDocumentSettings(writer);         // <o:OfficeDocumentSettings>
    writeExcelWorkbook(writer);                  // <x:ExcelWorkbook>
    writeStyles(writer);                         // <ss:Styles>
    writeWorksheet(writer, tableModel);          // <ss:Worksheet>

    writer.writeEndElement();                    // </ss:Workbook>
    writer.writeEndDocument();
    file.close();
}

void XmlSerializer::writeWorkbookStart(QXmlStreamWriter& writer)
{
    writer.writeStartDocument();
    writer.writeProcessingInstruction("mso-application", "progid=\"Excel.Sheet\"");

    writer.writeStartElement("ss:Workbook");
    writer.writeAttribute("xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
    writer.writeAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
    writer.writeAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");
    writer.writeAttribute("xmlns:html", "http://www.w3.org/TR/REC-html40");
}

void XmlSerializer::writeDocumentProperties(QXmlStreamWriter& writer)
{
    writer.writeStartElement("o:DocumentProperties");
    writer.writeAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
    writer.writeTextElement("o:Version", "16.00");
    writer.writeEndElement(); // o:DocumentProperties
}

void XmlSerializer::writeOfficeDocumentSettings(QXmlStreamWriter& writer)
{
    writer.writeStartElement("o:OfficeDocumentSettings");
    writer.writeAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
    writer.writeEmptyElement("o:AllowPNG");
    writer.writeEndElement(); // o:OfficeDocumentSettings
}

void XmlSerializer::writeExcelWorkbook(QXmlStreamWriter& writer)
{
    writer.writeStartElement("x:ExcelWorkbook");
    writer.writeAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");
    writer.writeTextElement("x:WindowHeight", "11985");
    writer.writeTextElement("x:WindowWidth", "28800");
    writer.writeTextElement("x:WindowTopX", "32767");
    writer.writeTextElement("x:WindowTopY", "32767");
    writer.writeTextElement("x:ProtectStructure", "False");
    writer.writeTextElement("x:ProtectWindows", "False");
    writer.writeEndElement(); // x:ExcelWorkbook
}

void XmlSerializer::writeStyles(QXmlStreamWriter& writer)
{
    writer.writeStartElement("ss:Styles");

    // Default style
    writer.writeStartElement("ss:Style");
    writer.writeAttribute("ss:ID", "Default");
    writer.writeAttribute("ss:Name", "Normal");

    writer.writeStartElement("ss:Alignment");
    writer.writeAttribute("ss:Vertical", "Bottom");
    writer.writeEndElement(); // ss:Alignment

    writer.writeStartElement("ss:Borders");
    writer.writeEndElement(); // ss:Borders

    writer.writeStartElement("ss:Font");
    writer.writeAttribute("ss:FontName", "Calibri");
    writer.writeAttribute("x:CharSet", "204");
    writer.writeAttribute("x:Family", "Swiss");
    writer.writeAttribute("ss:Size", "11");
    writer.writeAttribute("ss:Color", "#000000");
    writer.writeEndElement(); // ss:Font

    writer.writeStartElement("ss:Interior");
    writer.writeEndElement(); // ss:Interior

    writer.writeStartElement("ss:NumberFormat");
    writer.writeEndElement(); // ss:NumberFormat

    writer.writeStartElement("ss:Protection");
    writer.writeEndElement(); // ss:Protection
    writer.writeEndElement(); // ss:Styles

    writer.writeStartElement("ss:Style");
    writer.writeAttribute("ss:ID", "s62");
    writer.writeStartElement("ss:NumberFormat");
    writer.writeAttribute("ss:Format", "[$-F800]dddd\\,\\ mmmm\\ dd\\,\\ yyyy");
    writer.writeEndElement(); // NumberFormat
    writer.writeEndElement(); // Style

    writer.writeEndElement(); // ss:Styles
}

void XmlSerializer::writeWorksheet(QXmlStreamWriter& writer, TableModel* model)
{
    writer.writeStartElement("ss:Worksheet");
    writer.writeAttribute("ss:Name", "Sheet1");

    writer.writeStartElement("ss:Table");
    writer.writeAttribute("ss:ExpandedColumnCount", QString::number(model->columnCount()));
    writer.writeAttribute("ss:ExpandedRowCount", QString::number(model->rowCount() + 1)); // + заголовок
    writer.writeAttribute("x:FullColumns", "1");
    writer.writeAttribute("x:FullRows", "1");
    writer.writeAttribute("ss:DefaultRowHeight", "15");

    writeHeaderRow(writer, model);
    writeDataRows(writer, model);

    writer.writeEndElement(); // ss:Table

    writeWorksheetOptions(writer);

    writer.writeEndElement(); // ss:Worksheet
}

void XmlSerializer::writeHeaderRow(QXmlStreamWriter& writer, TableModel* model)
{
    writer.writeStartElement("ss:Row");

    for (int col = 0; col < model->columnCount(); ++col) {
        writer.writeStartElement("ss:Cell");
        QString header = model->headerData(col, Qt::Horizontal).toString();

        writer.writeStartElement("ss:Data");
        writer.writeAttribute("ss:Type", "String");
        writer.writeCharacters(header);
        writer.writeEndElement(); // ss:Data

        writer.writeEndElement(); // ss:Cell
    }

    writer.writeEndElement(); // ss:Row
}

void XmlSerializer::writeDataRows(QXmlStreamWriter& writer, TableModel* model)
{
    for (int row = 0; row < model->rowCount(); ++row) {
        writer.writeStartElement("ss:Row");

        int lastColIndex = 0;

        for (int col = 0; col < model->columnCount(); ++col) {
            QVariant data = model->index(row, col).data();

            if (!data.isValid() || data.isNull()) continue;

            int currentColIndex = col + 1;
            if (currentColIndex > lastColIndex + 1) {
                writer.writeStartElement("ss:Cell");
                writer.writeAttribute("ss:Index", QString::number(currentColIndex));
            } else {
                writer.writeStartElement("ss:Cell");
            }

            auto columnType = model->columnType(col);
            QString type = ColumnTypeUtils::toString(columnType);

            switch (columnType) {
            case ColumnTypeUtils::StringType:
                writer.writeStartElement("ss:Data");
                writer.writeAttribute("ss:Type", type);
                writer.writeCharacters(data.toString());
                break;
            case ColumnTypeUtils::NumberType:
                writer.writeStartElement("ss:Data");
                writer.writeAttribute("ss:Type", type);
                writer.writeCharacters(QString::number(data.toDouble()));
                break;
            case ColumnTypeUtils::BooleanType:
                writer.writeStartElement("ss:Data");
                writer.writeAttribute("ss:Type", type);
                writer.writeCharacters(data.toBool() ? "1" : "0");
                break;
            case ColumnTypeUtils::DateType:
                writer.writeAttribute("ss:StyleID", "s62");
                writer.writeStartElement("ss:Data");
                writer.writeAttribute("ss:Type", type);
                writer.writeCharacters(data.toDateTime().toString("yyyy-MM-ddTHH:mm:ss.zzz"));
                break;
            }

            writer.writeEndElement(); // ss:Data
            writer.writeEndElement(); // ss:Cell

            lastColIndex = currentColIndex;
        }

        writer.writeEndElement(); // ss:Row
    }
}

void XmlSerializer::writeWorksheetOptions(QXmlStreamWriter& writer)
{
    writer.writeStartElement("x:WorksheetOptions");
    writer.writeAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");

    writer.writeStartElement("x:Selected");
    writer.writeEndElement(); // x:Selected

    writer.writeStartElement("x:ProtectObjects");
    writer.writeCharacters("False");
    writer.writeEndElement(); // x:ProtectObjects

    writer.writeStartElement("x:ProtectScenarios");
    writer.writeCharacters("False");
    writer.writeEndElement(); // x:ProtectScenarios

    writer.writeEndElement(); // x:WorksheetOptions
}
