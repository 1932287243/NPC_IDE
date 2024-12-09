#include "filesystemmodel.h"

FileSystemModel::FileSystemModel(QObject *parent) : QFileSystemModel(parent)
{

}

Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    //只显示一列
    return 1;
}

bool FileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   if(value==fileName(index)) return true;
    QFileSystemModel::setData(index,value,role);
    emit dataChanged(index,index);
    return true;
}
