#ifndef __GPRMONITOR_H__
#define __GPRMONITOR_H__

#include <QTableWidget>
#include <QWidget>
#include <QLabel>

#include <QPair>
#include <QList>

class GPRMonitor:public QWidget
{
public:
    GPRMonitor(QWidget *parent);
    ~GPRMonitor();
    void initGPRMonitor();
    QList<QPair<int, int>> gpr_breakpoint;
    QTableWidget *gprTableWidget;
    void setFont(QString fontFamily, int fontSize);

public slots:
    void receiveGPRValue(unsigned int *gpr_val);

private:
    int last_gpr_pos;
    QLabel *title_label;
    void initLabel();
    void initTableWidget();

private slots:
    void setGPRBreakpoint(int row, int column);

};

#endif