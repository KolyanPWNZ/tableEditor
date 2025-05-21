#include "numberdelegate.h"
#include <QDoubleValidator>


QWidget * NumberDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QLineEdit* editor = new QLineEdit(parent);
    editor->setValidator(new QDoubleValidator(editor));
    return editor;
}

void NumberDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit* line = static_cast<QLineEdit*>(editor);
    line->setText(index.data().toString());
}

void NumberDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit* line = static_cast<QLineEdit*>(editor);
    model->setData(index, line->text().toDouble());
}
