#include "myhighlighter.h"
#include "mytextedit.h"
#include "ui_mytextedit.h"
#include <QDebug>

MyTextEdit::MyTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit)
{
    ui->setupUi(this);

    //绑定滚动条
    initConnect();

    //初始化字体
    initFont();

    //初始化高亮
    initHighlighter();

    //行高亮
    highlightCurrentLine();
}

MyTextEdit::~MyTextEdit()
{
    delete ui;
}


void MyTextEdit::initConnect()
{
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));

    connect(ui->textEdit->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textEditHScrollBarChanged()));
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),this,SLOT(scrollBarChanged()));

    connect(ui->textEdit->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textEditVScrollBarChanged()));
    connect(ui->textBrowser->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textBrowserVScrollBarChanged()));

    //cursor
    connect(ui->textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));
}

void MyTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor=ui->textEdit->textCursor();

    //selection.cursor.clearSelection();

    extraSelections.append(selection);

    ui->textEdit->setExtraSelections(extraSelections);

}

void MyTextEdit::initFont()
{
    mFont =QFont("Consolas",14);
    ui->textEdit->setFont(mFont);
    QTextBlockFormat format ;
    format.setLineHeight(QFontMetrics(mFont).height()*1.1,QTextBlockFormat::FixedHeight);

    QTextCursor cursor =  ui->textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    ui->textBrowser->setFont(mFont);
}

void MyTextEdit::initHighlighter()
{
    new MyHighlighter(ui->textEdit->document());
}

void MyTextEdit::textEditHScrollBarChanged()
{
    ui->horizontalScrollBar->setMinimum(ui->textEdit->horizontalScrollBar()->minimum());
    ui->horizontalScrollBar->setMaximum(ui->textEdit->horizontalScrollBar()->maximum());
    ui->horizontalScrollBar->setPageStep(ui->textEdit->horizontalScrollBar()->pageStep());
    ui->horizontalScrollBar->setValue(ui->textEdit->horizontalScrollBar()->value());
}

void MyTextEdit::scrollBarChanged()
{
    ui->textEdit->horizontalScrollBar()->setValue(ui->horizontalScrollBar->value());
}

void MyTextEdit::textEditVScrollBarChanged()
{
    ui->textBrowser->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->value());
}

void MyTextEdit::textBrowserVScrollBarChanged()
{
    ui->textEdit->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->value());
}

void MyTextEdit::onTextChanged()
{
    int lineCountOfTextEdit = ui->textEdit->document()->lineCount();
//    qDebug()<<"lineCountOfTextEdit:"<<lineCountOfTextEdit;
    QString text = ui->textBrowser->toPlainText();

    int lineCountOfTextBrowser = text.trimmed().split("\n").length();
//    qDebug()<<"lineCountOfTextBrowser:"<<lineCountOfTextBrowser;

    if(lineCountOfTextBrowser>lineCountOfTextEdit){
        for(int i = lineCountOfTextBrowser;i>lineCountOfTextEdit;i--){
            text.chop((QString::number(i)+"\n").length());
        }
    } else if (lineCountOfTextBrowser==1&&text.length()<1){
         text+= "1\n";
    }    else if(lineCountOfTextBrowser<lineCountOfTextEdit){
        for(int i = lineCountOfTextBrowser;i<lineCountOfTextEdit;i++){
            text+= QString::number(i+1)+"\n";
        }
    }

    ui->textBrowser->setMaximumWidth(25+QString::number(lineCountOfTextEdit).length()*7);
    ui->textBrowser->setText(text);

    QTextBlockFormat format ;
    format.setLineHeight(QFontMetrics(mFont).height()*1.1,QTextBlockFormat::FixedHeight);
    format.setAlignment(Qt::AlignRight);
    QTextCursor cursor =  ui->textBrowser->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}

