#include "mycodeeditor.h"
#include "mysplitterlayout.h"
#include <QMessageBox>
#include <QTextStream>
#include <QHeaderView>
#include <QDebug>

MySplitterLayout::MySplitterLayout(QWidget *parent,QString dir,QString fontFamily,int fontSize )
    :QSplitter(parent)
    ,mFontFamily(fontFamily)
    ,mFontSize(fontSize)
    ,mDir(dir)

{
    last_pc = 0x80000000;
    last_reg_pos = 0;
    pathLabel = createLabel();
    memLabel = createLabel();
    regLabel = createLabel();
    memLabel->setText("Memory Monitor");
    memLabel->setAlignment(Qt::AlignCenter);
    memLabel->setMaximumHeight(50);
    regLabel->setText("Register Monitor");
    regLabel->setAlignment(Qt::AlignCenter);
    regLabel->setMaximumHeight(50);

    mTreeView =createTreeView();

    QWidget * vLayoutWidget = new QWidget(this);
    QWidget * memLayoutWidget = new QWidget(this);
    QWidget * regLayoutWidget = new QWidget(this);

//    memLayoutWidget->resize(,200);

    mVLayout =createVLayout(vLayoutWidget);
    memVLayout = createVLayout(memLayoutWidget);
    regVLayout = createVLayout(regLayoutWidget);

    mVLayout->addWidget(pathLabel);
    mVLayout->addWidget(mTreeView);

    memTableWidget = new QTableWidget(this);
    regTableWidget = new QTableWidget(this);

    memVLayout->addWidget(memLabel);
    memVLayout->addWidget(memTableWidget);

    regVLayout->addWidget(regLabel);
    regVLayout->addWidget(regTableWidget);

    memSplitter = new QSplitter(Qt::Horizontal);
    regSplitter = new QSplitter(Qt::Vertical);

    fileLabelTabWidget =createTabWidget();

    memSplitter->addWidget(fileLabelTabWidget);
    memSplitter->addWidget(memLayoutWidget);
    memSplitter->setHandleWidth(0);
    regSplitter->addWidget(memSplitter);
    regSplitter->addWidget(regLayoutWidget);
    regSplitter->setHandleWidth(0);

    memSplitter->setStretchFactor(0,100);
    memSplitter->setStretchFactor(1,1);
    regSplitter->setStretchFactor(0,4);
    regSplitter->setStretchFactor(1,1);
    addWidget(vLayoutWidget);
    addWidget(regSplitter);

    //设置分割比距比例
    QList<int> sizeLists;
    sizeLists<<1000<<4000;
    setSizes(sizeLists);

    setHandleWidth(0);
    connect(memTableWidget, &QTableWidget::cellDoubleClicked, this, &MySplitterLayout::setMemBreakpoint);
    connect(regTableWidget, &QTableWidget::cellDoubleClicked, this, &MySplitterLayout::setRegBreakpoint);
}

void MySplitterLayout::createTab(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QFile::Text)){
        QMessageBox::warning(this,"警告","无法打开此文件:"+file.errorString());
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();


    int tabCount= fileLabelTabWidget->count();
    for(int index =0;index<tabCount;index++){
        qDebug()<<fileLabelTabWidget->tabText(index);
        if(fileLabelTabWidget->tabText(index)==filePath){
            return;
        }
    }

    //创建对象
    MyCodeEditor * codeEditor = new MyCodeEditor(this,QFont(mFontFamily,mFontSize));
    codeEditor->setPlainText(text);

    //设置文件名
    codeEditor->setFileName(filePath);
    //添加tab
    fileLabelTabWidget->addTab(codeEditor,filePath);
    //设置当前索引
    fileLabelTabWidget->setCurrentIndex(fileLabelTabWidget->count()-1);

    file.close();

    emit finishCreateTab(filePath);

}

void MySplitterLayout::onDataChanged(const QString &preFilePath,const QString &filePath)
{
    int count = fileLabelTabWidget->count();
    for(int i = 0; i<count ; i++){
        if(preFilePath==fileLabelTabWidget->tabText(i)){
            fileLabelTabWidget->setTabText(i,filePath);
            MyCodeEditor * codeEditor =( MyCodeEditor *) fileLabelTabWidget->widget(i);
            if(codeEditor)
                codeEditor->setFileName(filePath);
            return;
        }
    }
}

MyTreeView *MySplitterLayout::getTreeView()
{
    return mTreeView;
}

QLabel *MySplitterLayout::getLabel()
{
    return pathLabel;
}

QTabWidget *MySplitterLayout::getTabWidget()
{
    return fileLabelTabWidget;
}

void MySplitterLayout::setDir(const QString &dir)
{
    mDir = dir;
    mTreeView->setDir(dir);
    pathLabel->setText(dir);
}

void MySplitterLayout::setFont(const QFont &font)
{
    mFontFamily = font.family();
    mFontSize =font.pointSize();
    pathLabel->setFont(QFont(mFontFamily,mFontSize));
    memTableWidget->setFont(QFont(mFontFamily,mFontSize));
    regTableWidget->setFont(QFont(mFontFamily,mFontSize));

    MyCodeEditor * codeEditor = (MyCodeEditor *)fileLabelTabWidget->currentWidget();
    if(codeEditor)
        codeEditor->setAllFont(QFont(mFontFamily,mFontSize));
}


QLabel *MySplitterLayout::createLabel()
{
    QLabel * label = new QLabel(this);
    //设置label
    label->setFont(QFont(mFontFamily,mFontSize));
    label->setText(mDir);
    label->setIndent(5);
    label->setMinimumHeight(25);

    QPalette palette;
    palette.setColor(QPalette::Background,QColor(250,250,250));
    label->setPalette(palette);
    label->setAutoFillBackground(true);

    return label;
}

MyTreeView *MySplitterLayout::createTreeView()
{
    MyTreeView * treeView =   new MyTreeView(this,mDir);

    treeView->setFrameShape(QFrame::NoFrame);

    connect(treeView,SIGNAL(createTab(const QString &)),this,SLOT(createTab(const QString &)));

    connect(treeView,SIGNAL(onDataChanged(const QString & ,const QString &)),this,SLOT(onDataChanged(const QString & ,const QString &)));

    return treeView;
}


QVBoxLayout *MySplitterLayout::createVLayout(QWidget *parent)
{
    QVBoxLayout * vLayout = new QVBoxLayout(parent);

    vLayout->setMargin(0);
    vLayout->setSpacing(0);

    return vLayout;
}

QTabWidget *MySplitterLayout::createTabWidget()
{
    QTabWidget * tabWidget = new QTabWidget(this);

    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);

    connect(tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabWidgetCloseRequested(int)));
    return tabWidget;
}

void MySplitterLayout::onTabWidgetCloseRequested(int index)
{
    MyCodeEditor * codeEditor = (MyCodeEditor*)fileLabelTabWidget->currentWidget();
    if(!codeEditor->checkSaved()){
        QMessageBox::StandardButton btn = QMessageBox::question(this,"警告","您还没有保存文档！是否保存？",QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if(btn ==QMessageBox::Yes){
            if(codeEditor->saveFile()){
                fileLabelTabWidget->setTabText(fileLabelTabWidget->currentIndex(),codeEditor->getFileName());
            }
            return;
        }else if(btn ==QMessageBox::Cancel)
            return;
    }
    fileLabelTabWidget->removeTab(index);
    emit finishCreateTab(codeEditor->getFileName());
    delete codeEditor;
}

void MySplitterLayout::receiveNPCInfo(unsigned int pc, unsigned int inst)
{
    for (int col = 1; col < memTableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = memTableWidget->item((last_pc-0x80000000)/4, col);
        if (item) {
            item->setBackground(QColor(Qt::white));  // 设置背景为白色
        }
    }
    for (int col = 1; col < memTableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = memTableWidget->item((pc-0x80000000)/4, col);
        if (item) {
            item->setBackground(QColor(Qt::green));
        }
    }
    last_pc = pc;
    // 处理reg断点
    // for(int i = 0; i < mem_breakpoint.size(); ++i)
    // {
    //     QPair<int, int> pair = mem_breakpoint.at(i);
    //     if(pc == (0x80000000 + pair.first*4))
    //     {
    //         QString addr = QString::number(pc, 16).toUpper();
    //         QMessageBox::information(this, "breakpoint", "Trigger breakpoint.\npc = 0x"+addr);
    //         break;
    //     }
    // }
}

void MySplitterLayout::receiveRegInfo(unsigned int *reg_val)
{
    QTableWidgetItem *item_clear = regTableWidget->item(last_reg_pos/8, last_reg_pos%8*2+1);
    item_clear->setBackground(QColor(Qt::white)); 

    for(int i=0; i<32; i++)
    {
        QTableWidgetItem *item_color = regTableWidget->item(i/8, i%8*2+1);
        if (item_color) 
        {
            QString text = item_color->text();  
            bool ok;
            uint32_t value = text.toUInt(&ok, 16); 
            if (ok) 
            {
                if(value != reg_val[i])
                {
                    last_reg_pos = i;
                    QString val = QString("%1").arg(reg_val[i], 8, 16, QChar('0')).toUpper();
                    item_color->setText("0x"+val);
                    item_color->setBackground(QColor(Qt::green));  // 设置背景为绿色
                    item_color->setTextAlignment(Qt::AlignCenter);  // 文本居中
                }
            } 
            else 
            {
                qDebug() << "Conversion failed!";
            }
        } 
        else 
        {
            qDebug() << "Item not found!";
        }
    }
}

void MySplitterLayout::splitterlayoutInit(unsigned int * vmem, int mem_size)
{
    int i = 0;   
    memTableWidget->setRowCount(mem_size/4+1);// 设置行数和列数
    memTableWidget->setColumnCount(3);
    memTableWidget->setFont(QFont(mFontFamily,mFontSize));
    memTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表格显示网格线
    memTableWidget->setShowGrid(false);
    // 禁用选择功能
    memTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    memTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // memTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置表头标签
    memTableWidget->setHorizontalHeaderLabels(QStringList() << "Addr" << "Value" << "Asm");
    // 设置水平表头的高度
    memTableWidget->horizontalHeader()->setFixedHeight(50);  // 设置表头高度为 50 像素
    QFont headerFontMem = memTableWidget->horizontalHeader()->font();
    headerFontMem.setPointSize(16);  // 设置字体大小
    memTableWidget->horizontalHeader()->setFont(headerFontMem);

    while(1)
    {
        if(!vmem[i])
            break;    
        QString addr = QString::number(0x80000000+i*4, 16).toUpper();
        QTableWidgetItem *item_pc = new QTableWidgetItem("0x"+addr);
        item_pc->setTextAlignment(Qt::AlignCenter);  // 文本居中
        memTableWidget->setItem(i, 0, item_pc);

        QString inst = QString("%1").arg(vmem[i], 8, 16, QChar('0')).toUpper();
        QTableWidgetItem *item_inst = new QTableWidgetItem("0x"+inst);
        item_inst->setTextAlignment(Qt::AlignCenter);  // 文本居中
        memTableWidget->setItem(i, 1, item_inst);

        char p[50];
        disassemble(p, 50, 0x80000000+i*4, (uint8_t *)&vmem[i], 4);
        QTableWidgetItem *item_asm = new QTableWidgetItem(p);
        item_asm->setTextAlignment(Qt::AlignCenter);  // 文本居中
        memTableWidget->setItem(i, 2, item_asm);  
        memTableWidget->setRowHeight(i, 50);
        i++; 
    }

    // 设置样式表，使鼠标悬浮时高亮
    memTableWidget->setStyleSheet(
        "QTableWidget::item:hover {"
        "   background-color: lightblue;"  // 悬浮时的背景色
        "}"
        "QTableWidget::item:selected {"
        "   background-color: white;"  // 取消单击后的高亮
        "   color:black;"
        "}"
    );

    // 设置行数和列数
    regTableWidget->setRowCount(4);
    regTableWidget->setColumnCount(16);
    regTableWidget->setFont(QFont(mFontFamily,mFontSize));
    regTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    regTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    regTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 禁用选择功能
    regTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // regTableWidget->horizontalHeader()->hide();
    regTableWidget->verticalHeader()->hide();
    regTableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value" << \
                                                               "Name" << "Value" << "Name" << "Value");
        // 设置水平表头的高度
    regTableWidget->horizontalHeader()->setFixedHeight(50);  // 设置表头高度为 50 像素
    QFont headerFontReg = regTableWidget->horizontalHeader()->font();
    headerFontReg.setPointSize(16);  // 设置字体大小
    regTableWidget->horizontalHeader()->setFont(headerFontReg);

    for(int i=0; i<32; i++)
    {
        QTableWidgetItem *item_name = new QTableWidgetItem(reg[i]);
        item_name->setTextAlignment(Qt::AlignCenter);  // 文本居中
        regTableWidget->setItem(i/8, i%8*2, item_name);

        QString val = QString("%1").arg(0, 8, 16, QChar('0')).toUpper();
        QTableWidgetItem *item_val = new QTableWidgetItem("0x"+val);
        item_val->setTextAlignment(Qt::AlignCenter);  // 文本居中
        // item_val->setBackground(QColor(Qt::red)); 
        regTableWidget->setItem(i/8, i%8*2+1, item_val);
        regTableWidget->setRowHeight(i, 30);
        
    }
     // 设置样式表，使鼠标悬浮时高亮
    regTableWidget->setStyleSheet(
        "QTableWidget::item:hover {"
        "   background-color: lightblue;"  // 悬浮时的背景色
        "}"
    );
}

void MySplitterLayout::setMemBreakpoint(int row, int column)
{
    for(int i = 0; i <= mem_breakpoint.size(); ++i)
    {
        QPair<int, int> pair = mem_breakpoint.at(i);
        if(pair.first == row && pair.second == column)
        {
            mem_breakpoint.removeAt(i);
            QTableWidgetItem *item_mem = memTableWidget->item(row, 0);
            item_mem->setBackground(QColor(Qt::white));
            return;
        }
    }
    mem_breakpoint.append(qMakePair(row, column));
    QTableWidgetItem *item_mem = memTableWidget->item(row, 0);
    item_mem->setBackground(QColor(Qt::blue));
}

void MySplitterLayout::setRegBreakpoint(int row, int column)
{
    if(column%2==0)
    {
        for(int i = 0; i <= reg_breakpoint.size(); ++i)
        {
            QPair<int, int> pair = reg_breakpoint.at(i);
            if(pair.first == row && pair.second == column)
            {
                reg_breakpoint.removeAt(i);
                QTableWidgetItem *item_reg = regTableWidget->item(row, column);
                item_reg->setBackground(QColor(Qt::white));
                return;
            }
        }
        reg_breakpoint.append(qMakePair(row, column));
        QTableWidgetItem *item_reg = regTableWidget->item(row, column);
        item_reg->setBackground(QColor(Qt::blue));
    }
}

void MySplitterLayout::receiveDiffError()
{
    QTableWidgetItem *item = memTableWidget->item((last_pc-0x80000000)/4, 0);
    if (item) {
        item->setBackground(QColor(Qt::red)); 
    }
}