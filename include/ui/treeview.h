#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "filesystemmodel.h"

#include <QTreeView>
#include <QObject>
#include <QWidget>

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget * parent=nullptr,QString dir="E:/");

    void newFile();

    void setDir(const QString &dir);

private slots:
    void showCustomContextMenu(const QPoint &pos);
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onModelDataChanged(const QModelIndex &index);

signals:
    void createTab(const QString &filePath);
    void onDataChanged(const QString &preFilePath,const QString &filePath);

private:
    QString mFontFamily;
    int mFontSize;
    FileSystemModel * mFileSystemModel ;

    QString mDir;
    QString preFilePath;


    void setTreeViewModel();

};

#endif // TreeView_H
