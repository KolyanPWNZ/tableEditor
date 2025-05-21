#include "stringdelegate.h"
#include <QLineEdit>


QWidget * StringDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    return new QLineEdit(parent);
}

void StringDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit* line = static_cast<QLineEdit*>(editor);
    line->setText(index.data().toString());
}

void StringDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit* line = static_cast<QLineEdit*>(editor);
    model->setData(index, line->text());
}
