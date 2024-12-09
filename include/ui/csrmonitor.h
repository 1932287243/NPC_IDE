#ifndef __CSRMONITOR_H__
#define __CSRMONITOR_H__

#include <QTableWidget>
#include <QPair>
#include <QList>

class CSRMonitor:public QTableWidget
{
public:
    CSRMonitor();
    ~CSRMonitor();
    void initCSRMonitor();

    QList<QPair<int, int>> csr_breakpoint;

public slots:
    void receiveCSRValue(unsigned int *csr_val);

private:
    int last_csr_pos;

private slots:
    void setCSRBreakpoint(int row, int column);

};

#endif