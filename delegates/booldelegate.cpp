#include "booldelegate.h"
#include <QCheckBox>
#include <QMouseEvent>


QWidget * BoolDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QCheckBox* editor = new QCheckBox(parent);
    editor->setAutoFillBackground(true);
    return editor;
}

void BoolDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QCheckBox* checkBox = static_cast<QCheckBox*>(editor);
    checkBox->setChecked(index.data().toBool());
}

void BoolDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox* checkBox = static_cast<QCheckBox*>(editor);
    model->setData(index, checkBox->isChecked());
}

void BoolDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
