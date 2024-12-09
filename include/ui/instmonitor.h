#ifndef __INSTMONITOR_H__
#define __INSTMONITOR_H__

#include <QTableWidget>
#include <QPair>
#include <QList>
#include <QWidget>
#include <QLabel>

class InstMonitor:public QWidget
{
    Q_OBJECT
public:
    InstMonitor(QWidget *parent);
    ~InstMonitor();
    void InitInstMonitor(unsigned int * vmem, int inst_num);
    void setFont(QString fontFamily, int fontSize);

    QList<QPair<int, int>> mem_breakpoint;
    
public slots:
    void receiveNPCInfo(unsigned int pc);
    void setMemBreakpoint(int row, int column);
    void receiveDiffError();

private:
    unsigned int * vmem;
    int inst_num;
    uint32_t last_pc;
    QLabel *instMemLabel;
    QTableWidget *instTableWidget;

    void initTitle();
    void initTableWidget();
    
};

#endif