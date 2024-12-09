#include "memviewer.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QRegularExpressionValidator>
#include <QMessageBox>

MemViewer::MemViewer(QWidget *parent):
    QWidget(parent)
{
    // 设置正则表达式验证器，允许输入整数和以0x开头的十六进制数
    QRegularExpression regex("^(0x[0-9A-Fa-f]+|[0-9]+)$");  // 允许以 0x 开头的十六进制数和十进制整数
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);

    dataMemLabel = initLabel("Data Memory Monitor");
    dataMemLabel ->setAlignment(Qt::AlignCenter);
    dataMemLabel ->setMaximumHeight(50);

    // QWidget * dataMemLayoutWidget = new QWidget(parent);
    QWidget * dataInputLayoutWidget = new QWidget();
    QHBoxLayout * dataInputHLayout = new QHBoxLayout(dataInputLayoutWidget);
    dataInputHLayout->setMargin(0);
    dataInputHLayout->setSpacing(0);

    start_address =initLabel("Start Address:");
    start_address ->setAlignment(Qt::AlignCenter);
    start_address ->setMaximumHeight(50);

    addressLineEdit = new QLineEdit();
    addressLineEdit ->setPlaceholderText("0x80000000");
    addressLineEdit->setValidator(validator);
    QFont fontAddr = addressLineEdit->font();
    fontAddr.setPointSize(16);  // 设置字体大小为 14
    addressLineEdit->setFont(fontAddr);
    addressLineEdit->setMinimumHeight(30);  // 设置最小高度为 30 像素

    offset =initLabel("Offset:");
    offset ->setAlignment(Qt::AlignCenter);
    offset ->setMaximumHeight(50);

    offsetLineEdit = new QLineEdit();
    offsetLineEdit ->setPlaceholderText("10");
    offsetLineEdit->setValidator(validator);
    QFont fontOffset = offsetLineEdit->font();
    fontOffset.setPointSize(16);  // 设置字体大小为 14
    offsetLineEdit->setFont(fontOffset);
    offsetLineEdit->setMinimumHeight(30);  // 设置最小高度为 30 像素

    dataMemFindbutton = new QPushButton("Find");
    dataMemFindbutton->setMinimumHeight(30);

    dataInputHLayout->addWidget(start_address);
    dataInputHLayout->addWidget(addressLineEdit);
    dataInputHLayout->addWidget(offset);
    dataInputHLayout->addWidget(offsetLineEdit);
    dataInputHLayout->addWidget(dataMemFindbutton);
    
    QVBoxLayout * dataMemVLayout = new QVBoxLayout(this);
    dataMemVLayout->setMargin(0);
    dataMemVLayout->setSpacing(0);

    //data memory table
    dataMemTableWidget = new QTableWidget();
    InitTableWidget();

    dataMemVLayout->addWidget(dataMemLabel);
    dataMemVLayout->addWidget(dataInputLayoutWidget);
    dataMemVLayout->addWidget(dataMemTableWidget);
    connect(dataMemFindbutton, &QPushButton::clicked, this, &MemViewer::findDataMemToUI);
}

MemViewer::~MemViewer()
{
    
}

QLabel * MemViewer::initLabel(QString name)
{
    QLabel * label = new QLabel();
    //设置label
    label->setText(name);
    label->setIndent(5);
    label->setMinimumHeight(25);
    QPalette palette;
    palette.setColor(QPalette::Background,QColor(250,250,250));
    label->setPalette(palette);
    label->setAutoFillBackground(true);
    return label;
}

void MemViewer::InitTableWidget()
{
    dataMemTableWidget->setRowCount(10);// 设置行数和列数
    dataMemTableWidget->setColumnCount(5);
    dataMemTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表格显示网格线
    dataMemTableWidget->setShowGrid(false);
    // 禁用选择功能
    // this->setSelectionMode(QAbstractItemView::NoSelection);
    dataMemTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置表头标签
    dataMemTableWidget->setHorizontalHeaderLabels(QStringList() << "Addr" << "0" << "1" << "2" << "3");
    // 设置水平表头的高度
    dataMemTableWidget->horizontalHeader()->setFixedHeight(50);  // 设置表头高度为 50 像素
    QFont headerFontMem = dataMemTableWidget->horizontalHeader()->font();
    headerFontMem.setPointSize(16);  // 设置字体大小
    dataMemTableWidget->horizontalHeader()->setFont(headerFontMem);
}

void MemViewer::initVMem(uint8_t * vmem)
{
    vmem_addr = vmem;
}

void MemViewer::setFont(QString fontFamily, int fontSize)
{
    dataMemTableWidget->setFont(QFont(fontFamily, fontSize));
    dataMemLabel->setFont(QFont(fontFamily,fontSize));
    start_address->setFont(QFont(fontFamily,fontSize));
    offset->setFont(QFont(fontFamily,fontSize));
}

void MemViewer::findDataMemToUI()
{
    QString temp = addressLineEdit->text();
    temp.remove(0, 2);
    bool ok;
    unsigned int addr_val = temp.toUInt(&ok, 16);
    temp = offsetLineEdit->text();
    unsigned int offset = temp.toUInt();
    dataMemTableWidget->setRowCount(offset);
    unsigned int start_addr = (addr_val - 0x80000000)/2;
    if(addr_val < 0x80000000 ||  addr_val > 0x88000000)
    {
        QMessageBox::critical(nullptr, "Error", "physical memory area [ 0x80000000 ,  0x88000000 ]");
        return;
    }
    for(int i=0; i<offset; i++)
    {
        QString addr = QString::number(addr_val+i*4, 16).toUpper();
        QTableWidgetItem *item_addr = new QTableWidgetItem("0x"+addr);
        item_addr->setTextAlignment(Qt::AlignCenter);  // 文本居中
        
        dataMemTableWidget->setItem(i, 0, item_addr);
        for(int j=1; j<5; j++)
        {
            QString mem_dat = QString("%1").arg(vmem_addr[start_addr+i*4+(j-1)], 2, 16, QChar('0')).toUpper();
            QTableWidgetItem *item_dat = new QTableWidgetItem(mem_dat);
            item_dat->setTextAlignment(Qt::AlignCenter);  // 文本居中
            dataMemTableWidget->setItem(i, j, item_dat);
        }
        dataMemTableWidget->setRowHeight(i, 30);
    }
}