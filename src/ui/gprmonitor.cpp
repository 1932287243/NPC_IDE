#include <QHeaderView>
#include <iostream>
#include <QVBoxLayout>
#include "gprmonitor.h"

GPRMonitor::GPRMonitor(QWidget *parent)
    :QWidget(parent)
{
  
    last_gpr_pos = 0;

    initLabel();
    initTableWidget();
      
    QVBoxLayout * regVLayout = new QVBoxLayout(this);
    regVLayout->addWidget(title_label);
    regVLayout->addWidget(gprTableWidget);
    regVLayout->setMargin(0);
    regVLayout->setSpacing(0);

    connect(gprTableWidget, &QTableWidget::cellDoubleClicked, this, &GPRMonitor::setGPRBreakpoint);
}

GPRMonitor::~GPRMonitor()
{
    
}

void GPRMonitor::setFont(QString fontFamily, int fontSize)
{
    title_label->setFont(QFont(fontFamily, fontSize));
    gprTableWidget->setFont(QFont(fontFamily, fontSize));
}

void GPRMonitor::initLabel()
{
    title_label = new QLabel(this);
    title_label->setIndent(5);
    title_label->setMinimumHeight(25);

    QPalette palette;
    palette.setColor(QPalette::Background,QColor(250,250,250));
    title_label->setPalette(palette);
    title_label->setAutoFillBackground(true);

    title_label->setText("Register Monitor");
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setMaximumHeight(50);
}

void GPRMonitor::initTableWidget()
{
    gprTableWidget = new QTableWidget(this);
    // 设置行数和列数
    gprTableWidget->setRowCount(4);
    gprTableWidget->setColumnCount(16);

    gprTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    gprTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    gprTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 禁用选择功能
    gprTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // gprTableWidget->horizontalHeader()->hide();
    gprTableWidget->verticalHeader()->hide();
    gprTableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value");
    // 设置水平表头的高度
    gprTableWidget->horizontalHeader()->setFixedHeight(50);  // 设置表头高度为 50 像素
    QFont headerFontReg = gprTableWidget->horizontalHeader()->font();
    headerFontReg.setPointSize(16);  // 设置字体大小
    gprTableWidget->horizontalHeader()->setFont(headerFontReg);

    
    //  设置样式表，使鼠标悬浮时高亮
    gprTableWidget->setStyleSheet(
        "QTableWidget::item:hover {"
        "   background-color: lightblue;"  // 悬浮时的背景色
        "}"
    );
}

void GPRMonitor::initGPRMonitor()
{
    const QString reg[] = {
        "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    for(int i=0; i<32; i++)
    {
        QTableWidgetItem *item_name = new QTableWidgetItem(reg[i]);
        item_name->setTextAlignment(Qt::AlignCenter);  // 文本居中
        gprTableWidget->setItem(i/8, i%8*2, item_name);

        QString val = QString("%1").arg(0, 8, 16, QChar('0')).toUpper();
        QTableWidgetItem *item_val = new QTableWidgetItem("0x"+val);
        item_val->setTextAlignment(Qt::AlignCenter);  // 文本居中
        // item_val->setBackground(QColor(Qt::red)); 
        gprTableWidget->setItem(i/8, i%8*2+1, item_val);
        gprTableWidget->setRowHeight(i, 30); 
    }
}

void GPRMonitor::receiveGPRValue(unsigned int *gpr_val)
{
    QTableWidgetItem *item_clear = gprTableWidget->item(last_gpr_pos/8, last_gpr_pos%8*2+1);
    item_clear->setBackground(QColor(Qt::white)); 

    for(int i=0; i<32; i++)
    {
        QTableWidgetItem *item_color = gprTableWidget->item(i/8, i%8*2+1);
        if (item_color) 
        {
            QString text = item_color->text();  
            bool ok;
            uint32_t value = text.toUInt(&ok, 16); 
            if (ok) 
            {
                if(value != gpr_val[i])
                {
                    last_gpr_pos = i;
                    QString val = QString("%1").arg(gpr_val[i], 8, 16, QChar('0')).toUpper();
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

void GPRMonitor::setGPRBreakpoint(int row, int column)
{
    if(column%2==0)
    {
        for(int i = 0; i <= gpr_breakpoint.size(); ++i)
        {
            QPair<int, int> pair = gpr_breakpoint.at(i);
            if(pair.first == row && pair.second == column)
            {
                gpr_breakpoint.removeAt(i);
                QTableWidgetItem *item_reg = gprTableWidget->item(row, column);
                item_reg->setBackground(QColor(Qt::white));
                return;
            }
        }
        gpr_breakpoint.append(qMakePair(row, column));
        QTableWidgetItem *item_reg = gprTableWidget->item(row, column);
        item_reg->setBackground(QColor(Qt::blue));
    }
}