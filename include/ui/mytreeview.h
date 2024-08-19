#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include "myfilesystemmodel.h"

#include <QTreeView>
#include <QObject>
#include <QWidget>

class MyTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit MyTreeView(QWidget * parent=nullptr,QString dir="E:/");

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
    MyFileSystemModel * mFileSystemModel ;

    QString mDir;
    QString preFilePath;


    void setTreeViewModel();

};

#endif // MYTREEVIEW_H
