#include <QHeaderView>
#include <iostream>
#include "csrmonitor.h"


CSRMonitor::CSRMonitor()
{
// 设置行数和列数
    this->setRowCount(2);
    this->setColumnCount(8);
    // this->setFont(QFont(mFontFamily,mFontSize));
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 禁用选择功能
    this->setSelectionMode(QAbstractItemView::NoSelection);
    // this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setHorizontalHeaderLabels(QStringList() << "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value" );
    // 设置水平表头的高度
    this->horizontalHeader()->setFixedHeight(50);  // 设置表头高度为 50 像素
    QFont headerFontReg = this->horizontalHeader()->font();
    headerFontReg.setPointSize(16);  // 设置字体大小
    this->horizontalHeader()->setFont(headerFontReg);

    
    //  设置样式表，使鼠标悬浮时高亮
    this->setStyleSheet(
        "QTableWidget::item:hover {"
        "   background-color: lightblue;"  // 悬浮时的背景色
        "}"
    );

    connect(this, &QTableWidget::cellDoubleClicked, this, &CSRMonitor::setCSRBreakpoint);
}

CSRMonitor::~CSRMonitor()
{
    
}

void CSRMonitor::initCSRMonitor()
{
    const QString reg[] = {
        "mstatus", "mip", "mie", "mcause", "mtvec", "mtval", "mepc", "mscratch",
    };

    for(int i=0; i<8; i++)
    {
        QTableWidgetItem *item_name = new QTableWidgetItem(reg[i]);
        item_name->setTextAlignment(Qt::AlignCenter);  // 文本居中
        this->setItem(i/4, i%4*2, item_name);

        QString val = QString("%1").arg(0, 8, 16, QChar('0')).toUpper();
        QTableWidgetItem *item_val = new QTableWidgetItem("0x"+val);
        item_val->setTextAlignment(Qt::AlignCenter);  // 文本居中
        // item_val->setBackground(QColor(Qt::red)); 
        this->setItem(i/4, i%4*2+1, item_val);
        this->setRowHeight(i, 30); 
    }
}

void CSRMonitor::receiveCSRValue(unsigned int *csr_val)
{
    QTableWidgetItem *item_clear = this->item(last_csr_pos/8, last_csr_pos%8*2+1);
    item_clear->setBackground(QColor(Qt::white)); 

    for(int i=0; i<8; i++)
    {
        QTableWidgetItem *item_color = this->item(i/8, i%8*2+1);
        if (item_color) 
        {
            QString text = item_color->text();  
            bool ok;
            uint32_t value = text.toUInt(&ok, 16); 
            if (ok) 
            {
                if(value != csr_val[i])
                {
                    last_csr_pos = i;
                    QString val = QString("%1").arg(csr_val[i], 8, 16, QChar('0')).toUpper();
                    item_color->setText("0x"+val);
                    item_color->setBackground(QColor(Qt::green));  // 设置背景为绿色
                    item_color->setTextAlignment(Qt::AlignCenter);  // 文本居中
                }
            } 
            else 
            {
                std::cout << "Conversion failed!\n";
            }
        } 
        else 
        {
            std::cout << "Item not found\n";
        }
    }
}

void CSRMonitor::setCSRBreakpoint(int row, int column)
{
    if(column%2==0)
    {
        for(int i = 0; i <= csr_breakpoint.size(); ++i)
        {
            QPair<int, int> pair = csr_breakpoint.at(i);
            if(pair.first == row && pair.second == column)
            {
                csr_breakpoint.removeAt(i);
                QTableWidgetItem *item_reg = this->item(row, column);
                item_reg->setBackground(QColor(Qt::white));
                return;
            }
        }
        csr_breakpoint.append(qMakePair(row, column));
        QTableWidgetItem *item_reg = this->item(row, column);
        item_reg->setBackground(QColor(Qt::blue));
    }
}