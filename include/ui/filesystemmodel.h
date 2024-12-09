#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QObject>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = nullptr);

protected:
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};

#endif // FileSystemModel_H
