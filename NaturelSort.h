#ifndef NATURELSORT_H
#define NATURELSORT_H

#include <QSortFilterProxyModel>
#include <QDateTime>
#include "tablemodel.h"


class NaturalSort : public QSortFilterProxyModel {
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);

        int column = left.column();
        auto source = qobject_cast<const TableModel *>(sourceModel());
        ColumnTypeUtils::ColumnType type = source ? source->columnType(column) : ColumnTypeUtils::StringType;

        switch (type) {
        case ColumnTypeUtils::NumberType:
            return leftData.toDouble() < rightData.toDouble();
        case ColumnTypeUtils::DateType:
            return leftData.toDateTime() < rightData.toDateTime();
        case ColumnTypeUtils::BooleanType:
            return leftData.toBool() < rightData.toBool();
        case ColumnTypeUtils::StringType:
        default:
            return naturalCompare(leftData.toString(), rightData.toString()) < 0;
        }
    }

private:
    static int naturalCompare(const QString &a, const QString &b) {
        static const QRegularExpression re("(\\d+)|(\\D+)");
        QRegularExpressionMatchIterator iA = re.globalMatch(a);
        QRegularExpressionMatchIterator iB = re.globalMatch(b);

        while (iA.hasNext() && iB.hasNext()) {
            auto mA = iA.next();
            auto mB = iB.next();
            QString pA = mA.captured();
            QString pB = mB.captured();

            bool isNumA = mA.captured(1).length() > 0;
            bool isNumB = mB.captured(1).length() > 0;

            if (isNumA && isNumB) {
                int nA = pA.toInt();
                int nB = pB.toInt();
                if (nA != nB) return nA - nB;
            } else {
                int cmp = QString::compare(pA, pB, Qt::CaseInsensitive);
                if (cmp != 0) return cmp;
            }
        }

        return a.length() - b.length();
    }
};


#endif // NATURELSORT_H
