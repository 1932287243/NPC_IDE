#ifndef __MEMVIEWER_H__
#define __MEMVIEWER_H__

#include <QTableWidget>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <iostream>

class MemViewer:public QWidget
{
public:
    MemViewer(QWidget *parent);
    ~MemViewer();
    void initVMem(uint8_t * vmem);
    void setFont(QString fontFamily="Consolas",int fontSize=14);

private slots:
    void findDataMemToUI();

private:
    QLabel *dataMemLabel;

    QLabel *start_address;
    QLineEdit *addressLineEdit; 
    QLabel *offset;
    QLineEdit *offsetLineEdit;
    QPushButton *dataMemFindbutton;

    QTableWidget * dataMemTableWidget;

    uint8_t * vmem_addr;

    void InitTableWidget();
    QLabel * initLabel(QString name);
};

#endif