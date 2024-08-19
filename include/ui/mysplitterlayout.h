#ifndef MYSPLITTERLAYOUT_H
#define MYSPLITTERLAYOUT_H

#include "mytreeview.h"

#include <QSplitter>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPair>
#include <QList>

#include "disasm.h"

const QString reg[] = {
  "$0", "ra", "tp", "sp", "a0", "a1", "a2", "a3",
  "a4", "a5", "a6", "a7", "t0", "t1", "t2", "t3",
  "t4", "t5", "t6", "t7", "t8", "rs", "fp", "s0",
  "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8"
};

class MySplitterLayout : public QSplitter
{
    Q_OBJECT
public:
    explicit MySplitterLayout(QWidget * parent =nullptr,QString dir="E:/",QString fontFamily="Consolas",int fontSize=14);

    MyTreeView * getTreeView();

    QLabel *getLabel();

    QTabWidget *getTabWidget();

    void setDir(const QString &dir);

    void setFont(const QFont &font);
    
    void splitterlayoutInit(unsigned int * vmem, int mem_size);

    QList<QPair<int, int>> reg_breakpoint;
    QList<QPair<int, int>> mem_breakpoint;
    QTableWidget * regTableWidget;

public slots:
    void createTab(const QString &filePath);
    void onTabWidgetCloseRequested(int index);
    void receiveNPCInfo(unsigned int pc, unsigned int inst);
    void receiveRegInfo(unsigned int *reg_val);
    void receiveDiffError();

private slots:
    void onDataChanged(const QString &preFilePath,const QString &filePath);
    void setMemBreakpoint(int row, int column);
    void setRegBreakpoint(int row, int column);

signals:
    void finishCreateTab(const QString &filePath);

private:
    QLabel * createLabel();

    QVBoxLayout * createVLayout(QWidget * parent);

    QTabWidget * createTabWidget();

    MyTreeView * createTreeView();

    QString mFontFamily;
    int mFontSize;
    QString mDir;
    uint32_t last_pc;
    int last_reg_pos;


    QSplitter *memSplitter;   // 创建一个 内存  QSplitter
    QSplitter *regSplitter;   // 创建一个 寄存器 QSplitter

    QLabel *pathLabel ;
    QLabel *memLabel ;
    QLabel *regLabel ;

    MyTreeView * mTreeView ;
    QVBoxLayout * mVLayout;
    QVBoxLayout * memVLayout;
    QVBoxLayout * regVLayout;
    QTabWidget * fileLabelTabWidget;
    QTableWidget * memTableWidget;

};

#endif // MYSPLITTERLAYOUT_H
