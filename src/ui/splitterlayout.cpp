#include "codeeditor.h"
#include "splitterlayout.h"
#include <QMessageBox>
#include <QTextStream>
#include <QHeaderView>
#include <QDebug>

SplitterLayout::SplitterLayout(QWidget *parent,QString dir,QString fontFamily,int fontSize )
    :QSplitter(parent)
    ,mFontFamily(fontFamily)
    ,mFontSize(fontSize)
    ,mDir(dir)

{
    last_lineNumber = 0;

    //file directory tree
    pathLabel = createLabel();
    mTreeView =createTreeView();
    QWidget * vLayoutWidget = new QWidget(this);
    mVLayout =createVLayout(vLayoutWidget);
    mVLayout->addWidget(pathLabel);
    mVLayout->addWidget(mTreeView);
    
    //instruct memory tablewidget
    instMemWidget = new InstMonitor(this);
    dataMemLayoutWidget = new MemViewer(this);
    dataMemSplitter = new QSplitter(Qt::Vertical);

    dataMemSplitter ->addWidget(instMemWidget);
    dataMemSplitter ->addWidget(dataMemLayoutWidget);
    dataMemSplitter ->setHandleWidth(0);
    
    instMemSplitter = new QSplitter(Qt::Horizontal);
    // file edit region
    fileLabelTabWidget =createTabWidget();
    instMemSplitter->addWidget(fileLabelTabWidget);
    instMemSplitter->addWidget(dataMemSplitter);
    instMemSplitter->setHandleWidth(0);
   
    gprWidget = new GPRMonitor(this);

    regSplitter = new QSplitter(Qt::Vertical);
    regSplitter->addWidget(instMemSplitter);
    regSplitter->addWidget(gprWidget);
    regSplitter->setHandleWidth(0);

    instMemSplitter->setStretchFactor(0,250);
    instMemSplitter->setStretchFactor(1,1); 
    dataMemSplitter->setStretchFactor(0,5);
    dataMemSplitter->setStretchFactor(1,2); 
    regSplitter->setStretchFactor(0,5);
    regSplitter->setStretchFactor(1,2);

    addWidget(vLayoutWidget);
    addWidget(regSplitter);
    //设置分割比距比例
    QList<int> sizeLists;
    sizeLists<<1000<<4000;
    setSizes(sizeLists);
    setHandleWidth(0);
}

void SplitterLayout::createTab(const QString &filePath)
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
    CodeEditor * codeEditor = new CodeEditor(this,QFont(mFontFamily,mFontSize));
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

#include <QStringList>
#include <QRegExp>

void SplitterLayout::showSourceFromElf(QString src)
{
    int line_num = 0;
    // 按行分割字符串
    QStringList lines = src.split("\n");
    // 正则表达式匹配以数字开头的行
    QRegExp regex("^[0-9a-fA-F]{8}.*");

    // 筛选不匹配的行
    QStringList filteredLines;

    for (int i = 0; i < lines.size(); ++i) {
        if (!regex.exactMatch(lines[i])) {
            if((lines.size()>(i+1)) && (!lines[i].trimmed().isEmpty()) && regex.exactMatch(lines[i+1]))
            {
                bool ok;
                QString firstEightChars = lines[i+1].left(8);
                firstEightChars = QString("0x%1").arg(firstEightChars);
                // qDebug() << firstEightChars;
                unsigned int addr_val = firstEightChars.toUInt(&ok, 16);
                srcMapInst.append(qMakePair(line_num, addr_val));
            }

            line_num += 1;
            filteredLines.append(lines[i]);
        }
    }

    // 将过滤后的行重新连接为一个字符串
    QString filteredText = filteredLines.join("\n");


    //创建对象
    debugEditor = new CodeEditor(this,QFont(mFontFamily,mFontSize));
    debugEditor->setPlainText(filteredText);
    debugEditor->setReadOnly(true);
    //设置文件名
    debugEditor->setFileName("Debugging");
    //添加tab
    fileLabelTabWidget->addTab(debugEditor,"Debugging");
    //设置当前索引
    fileLabelTabWidget->setCurrentIndex(fileLabelTabWidget->count()-1);
}

void SplitterLayout::onDataChanged(const QString &preFilePath,const QString &filePath)
{
    int count = fileLabelTabWidget->count();
    for(int i = 0; i<count ; i++){
        if(preFilePath==fileLabelTabWidget->tabText(i)){
            fileLabelTabWidget->setTabText(i,filePath);
            CodeEditor * codeEditor =( CodeEditor *) fileLabelTabWidget->widget(i);
            if(codeEditor)
                codeEditor->setFileName(filePath);
            return;
        }
    }
}

TreeView *SplitterLayout::getTreeView()
{
    return mTreeView;
}

QLabel *SplitterLayout::getLabel()
{
    return pathLabel;
}

QTabWidget *SplitterLayout::getTabWidget()
{
    return fileLabelTabWidget;
}

void SplitterLayout::setDir(const QString &dir)
{
    mDir = dir;
    mTreeView->setDir(dir);
    pathLabel->setText(dir);
}

void SplitterLayout::setFont(const QFont &font)
{
    mFontFamily = font.family();
    mFontSize =font.pointSize();
    pathLabel->setFont(QFont(mFontFamily,mFontSize));
    instMemWidget->setFont(mFontFamily, mFontSize);
    gprWidget->setFont(mFontFamily,mFontSize);

    CodeEditor * codeEditor = (CodeEditor *)fileLabelTabWidget->currentWidget();
    if(codeEditor)
        codeEditor->setAllFont(QFont(mFontFamily,mFontSize));
}


QLabel *SplitterLayout::createLabel()
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

TreeView *SplitterLayout::createTreeView()
{
    TreeView * treeView =   new TreeView(this,mDir);

    treeView->setFrameShape(QFrame::NoFrame);

    connect(treeView,SIGNAL(createTab(const QString &)),this,SLOT(createTab(const QString &)));

    connect(treeView,SIGNAL(onDataChanged(const QString & ,const QString &)),this,SLOT(onDataChanged(const QString & ,const QString &)));

    return treeView;
}


QVBoxLayout *SplitterLayout::createVLayout(QWidget *parent)
{
    QVBoxLayout * vLayout = new QVBoxLayout(parent);

    vLayout->setMargin(0);
    vLayout->setSpacing(0);

    return vLayout;
}

QTabWidget *SplitterLayout::createTabWidget()
{
    QTabWidget * tabWidget = new QTabWidget(this);

    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);

    connect(tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabWidgetCloseRequested(int)));
    return tabWidget;
}

void SplitterLayout::onTabWidgetCloseRequested(int index)
{
    CodeEditor * codeEditor = (CodeEditor*)fileLabelTabWidget->currentWidget();
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

void SplitterLayout::splitterlayoutInit(unsigned int * vmem, int inst_num)
{
    instMemWidget->InitInstMonitor(vmem, inst_num);
    instMemWidget->setFont(mFontFamily, mFontSize);

    dataMemLayoutWidget->initVMem((uint8_t *)vmem);
    dataMemLayoutWidget->setFont(mFontFamily, mFontSize);
    
    gprWidget->initGPRMonitor();
    gprWidget->setFont(mFontFamily, mFontSize);
}

void SplitterLayout::showSrcMapInst(int line_num)
{
    qDebug() << line_num;
    // 取消所有行的高亮
    debugEditor->removeLastHighlights(last_lineNumber);
    debugEditor->highlightLine(line_num, Qt::green);
    last_lineNumber = line_num;
}
