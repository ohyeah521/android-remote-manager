#ifndef FILETREEITEM_H
#define FILETREEITEM_H

#include <QStandardItem>

class FileTreeItem:public QStandardItem
{
public:
    FileTreeItem():QStandardItem(){}
    explicit FileTreeItem(const QString &text):QStandardItem(text){}
    FileTreeItem(const QIcon &icon, const QString &text):QStandardItem(icon,text){}
    explicit FileTreeItem(int rows, int columns = 1):QStandardItem(rows,columns){}
    virtual ~FileTreeItem(){}

public:
    QString fileName;
};

#endif // FILETREEITEM_H
