#include "myhighlighter.h"
#include "mytexteditbycode.h"

#include <QVBoxLayout>
#include <QHBoxLayout>


MyTextEditByCode::MyTextEditByCode(QWidget *parent) : QWidget(parent)
{
    //UI组件
    initWidget();

    //字体
    initFont();

    //绑定
    initConnection();

    //高亮
    initHighlighter();

    //行的高亮
    highlightCurrentLine();
}

MyTextEditByCode::~MyTextEditByCode()
{
    delete textEdit;
    delete textBrowser;
    delete scrollBar;
}

void MyTextEditByCode::initWidget()
{
    QWidget * horizontalLayoutWidget = new QWidget();
    //竖向布局
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    verticalLayout->setSpacing(0);
    verticalLayout->setMargin(0);

    //横向布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setMargin(0);

    textEdit = new QTextEdit();
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);

    textBrowser = new QTextBrowser();
    textBrowser->setMaximumWidth(25);
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollBar = new QScrollBar();
    scrollBar->setOrientation(Qt::Horizontal);

    horizontalLayout->addWidget(textBrowser);
    horizontalLayout->addWidget(textEdit);

    verticalLayout->addWidget(horizontalLayoutWidget);
    verticalLayout->addWidget(scrollBar);
}

void MyTextEditByCode::initFont()
{
    mFont = QFont("Consolas",14);
    textEdit->setFont(mFont);
    QTextBlockFormat format ;
    format.setLineHeight(QFontMetrics(mFont).height()*1.1,QTextBlockFormat::FixedHeight);

    QTextCursor cursor =  textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);

    textBrowser->setFont(mFont);

}

void MyTextEditByCode::initConnection()
{
    //textChange
    connect(textEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));

    //滚动条
    connect(textEdit->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onTextEditHorizontalScrollBarChanged()));
    connect(textEdit->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onTextEditVerticalScrollBarChanged()));
    connect(scrollBar,SIGNAL(valueChanged(int)),this,SLOT(onScrollBarChanged()));
    connect(textBrowser->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onTextBrowserHorizontalScrollBarChanged()));

    //cursor
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));
}

void MyTextEditByCode::initHighlighter()
{
    new MyHighlighter(textEdit->document());
}

void MyTextEditByCode::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor=textEdit->textCursor();

    //selection.cursor.clearSelection();

    extraSelections.append(selection);

    textEdit->setExtraSelections(extraSelections);

}

void MyTextEditByCode::onTextEditHorizontalScrollBarChanged(){
    scrollBar->setMaximum(textEdit->horizontalScrollBar()->maximum());
    scrollBar->setMinimum(textEdit->horizontalScrollBar()->minimum());
    scrollBar->setPageStep(textEdit->horizontalScrollBar()->pageStep());
    scrollBar->setValue(textEdit->horizontalScrollBar()->value());
}

void MyTextEditByCode::onTextEditVerticalScrollBarChanged(){
    textBrowser->verticalScrollBar()->setMaximum(textEdit->verticalScrollBar()->maximum());
    textBrowser->verticalScrollBar()->setMinimum(textEdit->verticalScrollBar()->minimum());
    textBrowser->verticalScrollBar()->setPageStep(textEdit->verticalScrollBar()->pageStep());
    textBrowser->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value());
}

void MyTextEditByCode::onScrollBarChanged(){
    textEdit->horizontalScrollBar()->setValue(scrollBar->value());
}

void MyTextEditByCode::onTextBrowserHorizontalScrollBarChanged(){
    textEdit->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->value());
}

void MyTextEditByCode::onTextChanged()
{
    int lineContOfTextEdit = textEdit->document()->lineCount();

    QString text = textBrowser->toPlainText();
    int lineContOfTextBrowser = text.trimmed().split("\n").length();

    if(lineContOfTextBrowser>lineContOfTextEdit){
        for(int i = lineContOfTextBrowser;i>lineContOfTextEdit;i--)
            text.chop((QString::number(i)+"\n").length());
    }else if(lineContOfTextBrowser==1&text.length()<1){
        text ="1\n";
    }else if(lineContOfTextBrowser<lineContOfTextEdit){
        for(int i = lineContOfTextBrowser;i<lineContOfTextEdit;i++)
            text +=QString::number(i+1)+"\n";
    }
    textBrowser->setMaximumWidth(25+QString::number(lineContOfTextEdit).length()*7);
    textBrowser->setText(text);

    QTextBlockFormat format ;
    format.setLineHeight(QFontMetrics(mFont).height()*1.1,QTextBlockFormat::FixedHeight);
    format.setAlignment(Qt::AlignRight);
    QTextCursor cursor =  textBrowser->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}

