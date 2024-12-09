#include "codeeditor.h"
#include "treeview.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QClipboard>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>

TreeView::TreeView(QWidget * parent,QString dir)
    :QTreeView(parent)
    ,mFontFamily("Consolas")
    ,mFontSize(14)
    ,mFileSystemModel(0)
    ,mDir(dir)
{
    //设置model
    setTreeViewModel();

    //隐藏header
    setHeaderHidden(true);

    //设置右键菜单
    setContextMenuPolicy(Qt::CustomContextMenu);

    //添加connect
    //添加右键信号事件
    connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(showCustomContextMenu(const QPoint &)));

    //取消双击编辑
    setEditTriggers(QTreeView::NoEditTriggers);

    //绑定双击事件
    connect(this,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(onTreeViewDoubleClicked(const QModelIndex &)));


}

void TreeView::newFile()
{
    QModelIndex index = currentIndex();
    QString filePath =   mFileSystemModel->filePath(index);
    if(!mFileSystemModel->isDir(index)){
        filePath = filePath.left(filePath.length()-mFileSystemModel->fileName(index).length());
    }
    if(filePath.isEmpty()) return;
    QString fileName = filePath+(filePath.endsWith("/")?"":"/")+"新建文件"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")+".xjy";
    QFile file(fileName);
    if(!file.open(QIODevice::ReadWrite)){
        QMessageBox::critical(this,"失败","创建新文件失败!可能没有权限或文件已经存在！");
    }
    file.close();

    emit createTab(fileName);
}

void TreeView::setDir(const QString &dir)
{
    mDir=dir;
    setTreeViewModel();
}

//设置model
void TreeView::setTreeViewModel()
{
    if(mFileSystemModel) {
        delete mFileSystemModel;
    }
    //创建Model
    mFileSystemModel = new FileSystemModel();
    //设置根目录
    mFileSystemModel->setRootPath(mDir);
    //设置Model
    setModel(mFileSystemModel);

    //设置根目录
    setRootIndex(mFileSystemModel->index(mDir));

    //绑定dataChanged
    connect(mFileSystemModel,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),this,SLOT(onModelDataChanged(const QModelIndex &)));

}

//treeview的右键事件
void TreeView::showCustomContextMenu(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    QClipboard * clipboard =QApplication::clipboard();
    QMenu menu(this);
    if(mFileSystemModel->isDir(index)){
        qDebug()<< mFileSystemModel->filePath(index);
        //文件路径
        QString filePath = mFileSystemModel->filePath(index);
        if(filePath.isEmpty()) filePath = mDir;
        menu.addAction("新建文件(New File)",this,[=](){
            QString fileName = filePath+(filePath.endsWith("/")?"":"/")+"新建文件"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")+".xjy";
            QFile file(fileName);
            if(!file.open(QIODevice::ReadWrite)){
                QMessageBox::critical(this,"失败","创建新文件失败!可能没有权限或文件已经存在！");
            }
            file.close();
        });
        menu.addAction("新建文件夹(New Folder)",this,[=](){
            QDir dir(filePath);
            QString dirName = "新建文件夹"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
            if(!dir.exists(dirName))
                dir.mkdir(dirName);

        });

        menu.addSeparator();

        menu.addAction("重命名文件夹(Rename Dir)",this,[=](){
            //编辑节点
            edit(index);
        });
    }else{
        qDebug()<< mFileSystemModel->filePath(index);
        menu.addAction("重命名(Rename File)",this,[=](){
            //编辑节点
            edit(index);
            preFilePath =mFileSystemModel->filePath(index);
        });
        menu.addAction("删除文件(Remove File)",this,[=](){
            //删除文件
            //文件名称
            QString fileName = mFileSystemModel->fileName(index);
            //文件路径
            QString filePath = mFileSystemModel->filePath(index);
            if(QMessageBox::Yes==QMessageBox::question(this,"是否删除","请确认是否删除文件：\n   "+fileName+"\n此操作不可恢复!",QMessageBox::Yes|QMessageBox::No)){
                //删除
                if(!QFile::remove(filePath))
                    QMessageBox::critical(this,"失败","删除文件：\n   "+fileName+"\n失败!可能没有权限或文件不存在！");
            }
        });

        menu.addSeparator();
        menu.addAction("复制文件名(Copy File Name)",this,[=](){
            //复制文件名
            clipboard->setText(mFileSystemModel->fileName(index));

        });
    }

    menu.addAction("复制全路径(Copy Full Path)",this,[=](){
        //复制全路径
        clipboard->setText(mFileSystemModel->filePath(index));
    });

    menu.addSeparator();

    QString filePath =mFileSystemModel->filePath(index);
    if(!mFileSystemModel->isDir(index)) filePath = filePath.left(filePath.length()-mFileSystemModel->fileName(index).length());

    menu.addAction("在文件资源管理器中显示(Show In Explorer)",this,[=](){
        //在文件资源管理器中显示
        QDesktopServices::openUrl(QUrl(filePath));
    });

    menu.addAction("打开命令提示符(Open CMD)",this,[=](){
        //打开命令提示符
        QProcess * process = new QProcess(this);
        process->startDetached("cmd",QStringList()<<"/c"<<"cd "+filePath+" && start cmd");
        delete process;
    });

    menu.exec(QCursor::pos());
}

void TreeView::onTreeViewDoubleClicked(const QModelIndex &index)
{
    //打开文件操作
    //判断是否是文件
    if(!mFileSystemModel->isDir(index)){
        QString filePath = mFileSystemModel->filePath(index);
        emit createTab(filePath);
    }
}

void TreeView::onModelDataChanged(const QModelIndex &index)
{
    if(!mFileSystemModel->isDir(index))
        if(!preFilePath.isEmpty()){

            QString filePath = mFileSystemModel->filePath(index);
            emit onDataChanged(preFilePath,filePath);

            return;
        }
    setTreeViewModel();
}
