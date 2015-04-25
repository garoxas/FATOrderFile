#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QtWidgets/QTreeWidget>

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit TreeWidget(QWidget *parent = 0);
    ~TreeWidget();

private Q_SLOTS:
    void selectItems(const QList<QTreeWidgetItem *> items);

Q_SIGNALS:
    void itemDropped(QList<QTreeWidgetItem *> items);

protected:
    void dropEvent(QDropEvent *event);
};

Q_DECLARE_METATYPE(QList<QTreeWidgetItem *>)

#endif // TREEWIDGET_H
