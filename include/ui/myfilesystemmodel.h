#ifndef MYFILESYSTEMMODEL_H
#define MYFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QObject>

class MyFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit MyFileSystemModel(QObject *parent = nullptr);

protected:
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};

#endif // MYFILESYSTEMMODEL_H
