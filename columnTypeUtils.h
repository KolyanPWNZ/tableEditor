#ifndef COLUMNTYPEUTILS_H
#define COLUMNTYPEUTILS_H

#include <QString>
#include <QHash>

namespace ColumnTypeUtils {

enum ColumnType {
    StringType,
    NumberType,
    DateType,
    BooleanType,
    InvalidType
};

inline ColumnType fromString(const QString &typeStr) {
    static const QHash<QString, ColumnType> typeMap = {
        {"String",  StringType},
        {"Number",  NumberType},
        {"DateTime", DateType},
        {"Boolean", BooleanType}
    };
    return typeMap.value(typeStr, StringType);
}

inline QString toString(const ColumnType type) {
    static const QHash<ColumnType, QString> typeMap = {
        {StringType, "String"},
        {NumberType, "Number"},
        {DateType, "DateTime"},
        {BooleanType, "Boolean"}
    };
    return typeMap.value(type, "String");
}

}

#endif // COLUMNTYPEUTILS_H
