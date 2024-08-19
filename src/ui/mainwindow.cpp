#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QDateTime>
#include <QClipboard>
#include <QDesktopServices>
#include <QProcess>

unsigned int reg_file_val[32];

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#endif
#endif

MainWindow::MainWindow(int argc, char **argv, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mFontFamily("Consolas")
    , mFontSize(14)
    , mSplitterLayout(0)
    , mNPC(new NPC(argc, argv))
{
    ui->setupUi(this);
    mSettings = new QSettings("settings.ini",QSettings::IniFormat);
    mDir = mSettings->value("config/dir","E:/").toString();


    //初始化菜单
    initMenu();

    //初始化字体
    initFont();

    //初始化动作
    initAction(0);

    mSplitterLayout = new MySplitterLayout(this,mDir,mFontFamily,mFontSize);

    connect(mSplitterLayout,SIGNAL(finishCreateTab(const QString &)),this,SLOT(onFinishCreateTab(const QString &)));
    connect(mNPC, &NPC::sendNPCInfoToUI, this, &MainWindow::receiveNPCInfo);
    connect(mNPC, &NPC::sendNPCInfoToUI, mSplitterLayout, &MySplitterLayout::receiveNPCInfo);
    connect(this, &MainWindow::sendRegValueToUI, mSplitterLayout, &MySplitterLayout::receiveRegInfo);
    connect(mNPC, &NPC::destroyWindow, this, &MainWindow::onDestroyWindow);
    connect(mNPC, &NPC::sendDiffError, mSplitterLayout, &MySplitterLayout::receiveDiffError);
    // 创建一个布局并将 QSplitter 添加到布局中
    QVBoxLayout *layout = new QVBoxLayout(ui->page);
    layout->addWidget(mSplitterLayout);
    showFullScreen();
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);

#if !QT_CONFIG(printer)
    ui->print->setEnabled(false);
#endif
    mNPC->init_npc();
    mNPC->start();
    mSplitterLayout->splitterlayoutInit((unsigned int *)mem_addr, image_size);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mNPC;
}

void MainWindow::initFont(){
    mFontFamily = mSettings->value("config/font_family","Consolas").toString();
    mFontSize = mSettings->value("config/font_size",14).toInt();
}

void MainWindow::initAction(int tabCount)
{
    bool valid =tabCount>0;
    ui->save_file->setEnabled(valid);
    ui->save_as->setEnabled(valid);
    ui->copy->setEnabled(valid);
    ui->paste->setEnabled(valid);
    ui->cut->setEnabled(valid);
    ui->undo->setEnabled(valid);
    ui->redo->setEnabled(valid);
}

//初始化菜单
void MainWindow::initMenu()
{
    //获取Menu
    QMenu * recent = this->findChild<QMenu *>("recent");
    //获取Action
    QList<QObject *> chLists =recent->children();
    foreach(QObject * ch,chLists){
        QAction *action = (QAction *)ch;
        //清空子菜单Action
        recent->removeAction(action);
    }

    QList<QString> lists = getHistory();


    for(int i=lists.size()-1 ;i>=0;i--){
        //生成子菜单Action
        recent->addAction(lists[i],this,&MainWindow::on_open_rencent_file);
    }

    if(lists.size()>0){
        recent->addAction("清除历史记录",this,&MainWindow::on_clear_history_triggered,QKeySequence("Ctrl+Alt+Shift+C"));
    }

}


//获取历史记录
QList<QString> MainWindow::getHistory(){
    //打开开始读
    int size =mSettings->beginReadArray("history");

    //创建返回对象
    QList<QString> lists;
    for(int i = 0;i<size;i++){
        mSettings->setArrayIndex(i);
        QString path = mSettings->value("path").toString();
        lists.append(path);
    }
    //关闭读
    mSettings->endArray();

    return lists;
}


//保存打开历史记录
void MainWindow::saveHistory(QString path){

    //获取历史
    QList<QString> lists = getHistory();
    foreach(QString str,lists){
        if(str==path){
            lists.removeOne(str);
        }
    }
    lists.append(path);

    //打开开始写入
    mSettings->beginWriteArray("history");
    for(int i =0;i<lists.size();i++){
        mSettings->setArrayIndex(i);
        //保存字符串
        mSettings->setValue("path",lists[i]);
    }

    //关闭开始写入
    mSettings->endArray();
}



//新建文件
void MainWindow::on_new_file_triggered()
{
    mSplitterLayout->getTreeView()->newFile();

}


//打开最近文件
void MainWindow::on_open_rencent_file()
{
    mSplitterLayout->createTab(((QAction *)sender())->text());
}

//打开文件
void MainWindow::on_open_dir_triggered()
{
    QString dirFromSetttings = mSettings->value("dir",mDir).toString();
    QString dir = QFileDialog::getExistingDirectory(this,"选择文件夹",dirFromSetttings,QFileDialog::ShowDirsOnly);
    if(dir.isEmpty()) return;
    mSettings->setValue("config/dir",dir);
    mDir = dir;
    mSplitterLayout->setDir(mDir);
//    setTreeViewModel();
   mSplitterLayout->getLabel()->setText(mDir);
}


//保存文件
void MainWindow::on_save_file_triggered()
{
    //把保存交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        if(codeEditor->saveFile()){
            saveSuccessAction(codeEditor);
        }
    }
}

//另存为
void MainWindow::on_save_as_triggered()
{
    //把另存为交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        if(codeEditor->saveAsFile()){
            saveSuccessAction(codeEditor);
        }
    }
}

//复制
void MainWindow::on_copy_triggered()
{
    //把复制交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        codeEditor->copy();
    }
}

//粘贴
void MainWindow::on_paste_triggered()
{
    //把粘贴交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        codeEditor->paste();
    }
}

//剪切
void MainWindow::on_cut_triggered()
{
    //把剪切交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        codeEditor->cut();
    }
}

//撤销
void MainWindow::on_undo_triggered()
{
    //把撤销交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        codeEditor->undo();
    }
}

//取消撤销
void MainWindow::on_redo_triggered()
{
    //把取消撤销交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
        codeEditor->redo();
    }
}

//字体
void MainWindow::on_font_triggered()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected,QFont(mFontFamily,mFontSize),this);
    if(fontSelected){
        mSplitterLayout->setFont(QFont(font.family(),font.pointSize()));
        mSettings->setValue("config/font_family",font.family());
        mSettings->setValue("config/font_size",font.pointSize());
    }
}

//打印
void MainWindow::on_print_triggered()
{
    //把打印交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )mSplitterLayout->getTabWidget()->currentWidget();
    if(codeEditor){
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
        QPrinter printDev;
#if QT_CONFIG(printdialog)
        QPrintDialog dialog(&printDev,this);
        if(dialog.exec()==QDialog::Rejected)
            return;
#endif
        codeEditor->print(&printDev);
#endif
    }
}

//关于
void MainWindow::on_about_triggered()
{
    QMessageBox::about(this,"这是我的Notepad！","这是我的Notepad，欢迎学习和使用！");
}


//退出
void MainWindow::on_exit_triggered()
{
    QCoreApplication::exit();
}


void MainWindow::on_clear_history_triggered()
{
    mSettings->remove("history");
    initMenu();
}

void MainWindow::saveSuccessAction( MyCodeEditor * codeEditor)
{
    QString fileName  = codeEditor->getFileName();
    //保存历史
    saveHistory(fileName);
    //设置tab标题
    mSplitterLayout->getTabWidget()->setTabText(mSplitterLayout->getTabWidget()->currentIndex(),fileName);
    //初始化菜单
    initMenu();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(mSplitterLayout->getTabWidget()->count()>0){
        QMessageBox::question(this,
                              "警告",
                              "有未保存的文件，确定要关闭吗？",
                              QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes?event->accept():event->ignore();
    }
    // emit quitSBD();
    onDestroyWindow();
}

void MainWindow::onFinishCreateTab(const QString &filePath)
{
    saveHistory(filePath);
    initMenu();
    initAction(mSplitterLayout->getTabWidget()->count());
}

void MainWindow::onItemClicked(QListWidgetItem *item)
{
//    if (item)
//    {
//        qDebug() << "Clicked item text:" << item->text();
//    }
    Q_UNUSED(item)

    // 判断点击了哪一个 QListWidget
    if (ui->listWidget->currentRow() != -1)
    {
//        qDebug() << "Clicked QListWidget:" << ui->listWidget->currentRow() + 1;
        ui->stackedWidget->setCurrentIndex(ui->listWidget->currentRow());
    }
}

void MainWindow::receiveNPCInfo(unsigned int pc, unsigned int inst)
{
    // 将 VlUnpacked 数组转换为标准的 unsigned int 数组
    for (int i = 0; i < 32; ++i) {
        reg_file_val[i] = static_cast<unsigned int>(mNPC->top->rootp->CPU__DOT__reg_file__DOT__rf[i]);
    }
    emit sendRegValueToUI(reg_file_val);
}

void MainWindow::onDestroyWindow() 
{
    this->deleteLater();
}