#include "datedelegate.h"
#include <QDateEdit>


QWidget * DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QDateEdit* editor = new QDateEdit(parent);
    editor->setDisplayFormat("dd-MM-yyyy");
    editor->setCalendarPopup(true);
    return editor;
}

void DateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->setDate(index.data().toDate());
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
    model->setData(index, dateEdit->date());
}
