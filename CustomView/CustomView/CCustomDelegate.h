#pragma once

#include <QStyledItemDelegate>

class CCheckBoxCustomDelegate  : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CCheckBoxCustomDelegate(QObject *parent);
    ~CCheckBoxCustomDelegate();


    // editing
    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

class CLineEditCustomDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CLineEditCustomDelegate(QObject* parent);
    ~CLineEditCustomDelegate();


    // editing
    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

class CComboBoxCustomDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CComboBoxCustomDelegate(QObject* parent);
    ~CComboBoxCustomDelegate();


    // editing
    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};
