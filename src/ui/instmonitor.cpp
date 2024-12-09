#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>
#include "instmonitor.h"
#include "disasm.h"

InstMonitor::InstMonitor(QWidget *parent)
    :QWidget(parent)
{
    last_pc = 0x80000000;

    initTitle();
    initTableWidget();
    
    QVBoxLayout * instMemVLayout = new QVBoxLayout(this);
    instMemVLayout->setMargin(0);
    instMemVLayout->setSpacing(0);

    instMemVLayout->addWidget(instMemLabel);
    instMemVLayout->addWidget(instTableWidget);
    
    connect(instTableWidget, &QTableWidget::cellDoubleClicked, this, &InstMonitor::setMemBreakpoint);
}

InstMonitor::~InstMonitor()
{
    
}

void InstMonitor::initTitle()
{
    instMemLabel = new QLabel(this);
  
    instMemLabel->setIndent(5);
    instMemLabel->setMinimumHeight(25);

    QPalette palette;
    palette.setColor(QPalette::Background,QColor(250,250,250));
    instMemLabel->setPalette(palette);
    instMemLabel->setAutoFillBackground(true);

    instMemLabel->setText("Instruct Memory Monitor");
    instMemLabel->setAlignment(Qt::AlignCenter);
    instMemLabel->setMaximumHeight(50);
}

void InstMonitor::initTableWidget()
{
    instTableWidget = new QTableWidget(this);

    instTableWidget->setColumnCount(4);
    // 
    instTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表格显示网格线
    instTableWidget->setShowGrid(false);
    // 禁用选择功能
    instTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    instTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // instTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置表头标签
    instTableWidget->setHorizontalHeaderLabels(QStringList() << "" << "Addr" << "Value" << "Asm");
    // 设置水平表头的高度
    instTableWidget->horizontalHeader()->setFixedHeight(50);  // 设置表头高度为 50 像素
    instTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    // instTableWidget->horizontalHeader()->resizeSection(1, QHeaderView::ResizeToContents);

    QFont headerFontMem = instTableWidget->horizontalHeader()->font();
    headerFontMem.setPointSize(16);  // 设置字体大小
    instTableWidget->horizontalHeader()->setFont(headerFontMem);

    // 设置样式表，使鼠标悬浮时高亮
    instTableWidget->setStyleSheet(
        "QTableWidget::item:hover {"
        "   background-color: lightblue;"  // 悬浮时的背景色
        "}"
        "QTableWidget::item:selected {"
        "   background-color: white;"  // 取消单击后的高亮
        "   color:black;"
        "}"
    );
}

void InstMonitor::setFont(QString fontFamily, int fontSize)
{
    instTableWidget->setFont(QFont(fontFamily,fontSize+3));
    instMemLabel->setFont(QFont(fontFamily,fontSize+6));
}

void InstMonitor::InitInstMonitor(unsigned int * vmem, int inst_num)
{
    int i = 0;   
    instTableWidget->setRowCount(inst_num);// 设置行数
    while(1)
    {
        if(i==inst_num)
            break;    
        QString addr = QString::number(0x80000000+i*4, 16).toUpper();
        QTableWidgetItem *item_pc = new QTableWidgetItem("0x"+addr);
        item_pc->setTextAlignment(Qt::AlignCenter);  // 文本居中
        instTableWidget->setItem(i, 1, item_pc);

        QString inst = QString("%1").arg(vmem[i], 8, 16, QChar('0')).toUpper();
        QTableWidgetItem *item_inst = new QTableWidgetItem("0x"+inst);
        item_inst->setTextAlignment(Qt::AlignCenter);  // 文本居中
        instTableWidget->setItem(i, 2, item_inst);

        char p[50];
        disassemble(p, 50, 0x80000000+i*4, (uint8_t *)&vmem[i], 4);
        QTableWidgetItem *item_asm = new QTableWidgetItem(p);
        item_asm->setTextAlignment(Qt::AlignCenter);  // 文本居中
        instTableWidget->setItem(i, 3, item_asm);  
        instTableWidget->setRowHeight(i, 50);
        i++; 
    }
}

// receive pc values and set the monitor to highlight the corresponding pc values
void InstMonitor::receiveNPCInfo(unsigned int pc)
{
    for (int col = 1; col < instTableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = instTableWidget->item((last_pc-0x80000000)/4, col);
        if (item) {
            item->setBackground(QColor(Qt::white));  // 设置背景为白色
        }
    }
    for (int col = 1; col < instTableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = instTableWidget->item((pc-0x80000000)/4, col);
        if (item) {
            item->setBackground(QColor(Qt::green));
            instTableWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter); 
        }
    }
    last_pc = pc;
}

void InstMonitor::setMemBreakpoint(int row, int column)
{
    for(int i = 0; i <= mem_breakpoint.size(); ++i)
    {
        QPair<int, int> pair = mem_breakpoint.at(i);
        if(pair.first == row && pair.second == column)
        {
            mem_breakpoint.removeAt(i);
            // 获取指定单元格中的QWidge
            QWidget* cellWidget = instTableWidget->cellWidget(row, 0);
            if (cellWidget) {
                QLabel* label = qobject_cast<QLabel*>(cellWidget);
                if (label) {
                    label->clear();
                }
                // std::cout << row << std::endl;
                // delete label;
            }
            else {
                std::cout << "Failed to load image!";
            }
            return;
        }
    }
    mem_breakpoint.append(qMakePair(row, column));

    QPixmap pixmap(":/images/images/breakpoint.png");  // 使用qrc中的路径
    // 调整图片的大小为tablewidget的高，保持图像的长宽比
    pixmap = pixmap.scaled(instTableWidget->rowHeight(row), instTableWidget->rowHeight(row), Qt::KeepAspectRatio);
    // 创建一个QLabel并设置标签文本
    QLabel *label = new QLabel;
    label->setPixmap(pixmap);  // 将图片显示在QLabel上
    // 将QLabel放置到指定的单元格 
    instTableWidget->setCellWidget(row, 0, label);

    // if (!pixmap.isNull()) {
    //     // 获取指定单元格中的QWidget
    //     QWidget* cellWidget = this->cellWidget(row, 0);
    //     if (cellWidget) {
    //         QLabel* label = qobject_cast<QLabel*>(cellWidget);
    //         if (label) {
    //             label->setPixmap(pixmap);  // 将图片显示在QLabel上
    //         }
    //     }
    // } 
    // else {
    //     std::cout << "Failed to load image!";
    // }
}

void InstMonitor::receiveDiffError()
{
    QPixmap pixmap(":/images/images/triggerbp.png");  // 使用qrc中的路径
    // 调整图片的大小为tablewidget的高，保持图像的长宽比
    pixmap = pixmap.scaled(instTableWidget->rowHeight(0), instTableWidget->rowHeight(0), Qt::KeepAspectRatio);

     if (!pixmap.isNull()) {
        // 获取指定单元格中的QWidget
        QWidget* cellWidget = instTableWidget->cellWidget((last_pc-0x80000000)/4, 0);
        if (cellWidget) {
            QLabel* label = qobject_cast<QLabel*>(cellWidget);
            if (label) {
                label->setPixmap(pixmap);  // 将图片显示在QLabel上
            }
        }
    } 
    else {
        std::cout << "Failed to load image!";
    }
}
