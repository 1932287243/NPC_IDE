#ifndef SPLITTERLAYOUT_H
#define SPLITTERLAYOUT_H

#include "treeview.h"

#include <QSplitter>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QPair>
#include <QList>

#include "disasm.h"
#include "instmonitor.h"
#include "gprmonitor.h"
#include "memviewer.h"

class InstMonitor;
class GPRMonitor;
class MemViewer;

class SplitterLayout : public QSplitter
{
    Q_OBJECT
public:
    explicit SplitterLayout(QWidget * parent =nullptr,QString dir="E:/",QString fontFamily="Consolas",int fontSize=14);

    TreeView * getTreeView();

    QLabel *getLabel();

    QTabWidget *getTabWidget();

    void setDir(const QString &dir);

    void setFont(const QFont &font);
    
    void splitterlayoutInit(unsigned int * vmem, int inst_num);

    void showSourceFromElf(QString src);

    void showSrcMapInst(int line_num);

    GPRMonitor *gprWidget;
    InstMonitor *instMemWidget;
    MemViewer *dataMemLayoutWidget;

    QList<QPair<int, int>> srcMapInst;

public slots:
    void createTab(const QString &filePath);
    void onTabWidgetCloseRequested(int index);

private slots:
    void onDataChanged(const QString &preFilePath,const QString &filePath);

signals:
    void finishCreateTab(const QString &filePath);

private:
    QLabel * createLabel();

    QVBoxLayout * createVLayout(QWidget * parent);

    QTabWidget * createTabWidget();

    TreeView * createTreeView();

    QString mFontFamily;
    int mFontSize;
    QString mDir;
    int last_lineNumber;

    QRegularExpressionValidator *validator;
    QPushButton *dataMemFindbutton;
    QSplitter *instMemSplitter;   // 创建一个 指令内存  QSplitter
    QSplitter *dataMemSplitter;   // 创建一个 数据内存  QSplitter
    QSplitter *regSplitter;   // 创建一个 寄存器 QSplitter

    QLabel *pathLabel ;
    QLabel *instMemLabel ;
    QLabel *dataMemLabel ;
    QLabel *regLabel ;

    TreeView * mTreeView ;
    QVBoxLayout * mVLayout;
    QVBoxLayout * instMemVLayout;
    QVBoxLayout * dataMemVLayout;
    QHBoxLayout * dataInputHLayout;

    QVBoxLayout * regVLayout;
    QTabWidget * fileLabelTabWidget;

    CodeEditor * debugEditor;

};

#endif // SplitterLayout_H
