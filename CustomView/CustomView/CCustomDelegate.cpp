#include "CCustomDelegate.h"

CLineEditCustomDelegate::CLineEditCustomDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

CLineEditCustomDelegate::~CLineEditCustomDelegate()
{
}

QWidget* CLineEditCustomDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return nullptr;
}

void CLineEditCustomDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
}

void CLineEditCustomDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
}

void CLineEditCustomDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
}


