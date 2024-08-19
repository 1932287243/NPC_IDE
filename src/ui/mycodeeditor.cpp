#include "mycodeeditor.h"
#include "myhighlighter.h"

#include <QPainter>
#include <QTextStream>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QAbstractItemView>

#include<QDebug>

MyCodeEditor::MyCodeEditor(QWidget *parent,QFont font)
    : QPlainTextEdit(parent)
    , lineNumberWidget(0)
    , mHighlighter(0)
    , mCompleter(0)
{
    lineNumberWidget = new LineNumberWidget(this);

    //绑定
    initConnection();

    //高亮
    initHighlighter();

    //设置completor
    initCompleter();


    //设置字体
    setAllFont(font);

    //行高亮
    highlightCurrentLine();

    //设置边距
    updateLineNumberWidgetWidth();

    //不自动换行
    setLineWrapMode(QPlainTextEdit::NoWrap);

    //去掉边框
    setFrameShape(QPlainTextEdit::NoFrame);

}

MyCodeEditor::~MyCodeEditor()
{
    // qDebug() << "MycodeEditor" ;
    delete  lineNumberWidget;
}

void MyCodeEditor::initConnection()
{
    //cursor
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));

    //textChanged
    connect(this,SIGNAL(textChanged()),this,SLOT(updateSaveState()));

    //blockCount
    connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberWidgetWidth()));

    //updateRequest
    connect(this,SIGNAL(updateRequest(QRect,int)),this,SLOT(updateLineNumberWidget(QRect,int)));
}

void MyCodeEditor::setAllFont(QFont font)
{
    this->setFont(font);
    mHighlighter->setFont(font);
    mHighlighter->rehighlight();
    mCompleter->popup()->setFont(font);
    viewport()->update();
    lineNumberWidget->update();
    updateLineNumberWidgetWidth();
}

bool MyCodeEditor::checkSaved()
{
    return isSaved;
}

void MyCodeEditor::initHighlighter()
{
    mHighlighter =  new MyHighlighter(document());
}

void MyCodeEditor::initCompleter()
{
    mCompleter = new QCompleter();

    QStringList list;
    list<<"int"<<"init"<<"void"<<"while"<<"completer"<<"abstract"<<"apple";
    list.sort(Qt::CaseInsensitive);

    QStringListModel * model = new QStringListModel(list,mCompleter);

    mCompleter->setModel(model);
    mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    mCompleter->setWrapAround(false);

    mCompleter->setWidget(this);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);

    connect(mCompleter,SIGNAL(activated(const QString &)),this,SLOT(insertCompletion(const QString &)));
}

int MyCodeEditor::getLineNumberWidgetWidth()
{
    //获取宽度（合适）
    return 8+QString::number(blockCount()+1).length()*fontMetrics().horizontalAdvance(QChar('0'));
}


void MyCodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor =textCursor();

    extraSelections.append(selection);
    setExtraSelections(extraSelections);

}

void MyCodeEditor::updateLineNumberWidget(QRect rect, int dy)
{
    if(dy)
        lineNumberWidget->scroll(0,dy);
    else
        lineNumberWidget->update(0,rect.y(),getLineNumberWidgetWidth(),rect.height());
}

void MyCodeEditor::updateLineNumberWidgetWidth()
{
    //设置边距
    setViewportMargins(getLineNumberWidgetWidth(),0,0,0);
}

void MyCodeEditor::updateSaveState()
{
    //更新保存状态
    isSaved = false;
}

void MyCodeEditor::insertCompletion(const QString &completion)
{
    QTextCursor cursor = textCursor();



    int length =completion.length()-mCompleter->completionPrefix().length();

    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);

    cursor.insertText(completion.right(length));

    setTextCursor(cursor);
}

void MyCodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    lineNumberWidget->setGeometry(0,0,getLineNumberWidgetWidth(),contentsRect().height());
}

void MyCodeEditor::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_ParenLeft:
        textCursor().insertText(")");
        break;
    case Qt::Key_BracketLeft:
        textCursor().insertText("]");
        break;
    case Qt::Key_BraceLeft:
        textCursor().insertText("}");
        break;
    case Qt::Key_QuoteDbl:
        textCursor().insertText("\"");
        break;
    case Qt::Key_Apostrophe:
        textCursor().insertText("'");
        break;
    case Qt::Key_Enter:
    case Qt::Key_Backtab:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Tab:
        if(mCompleter->popup()->isVisible()){
            event->ignore();
            return;
        }
    }
    //Alt + C
    bool isShortCut = (event->modifiers()&Qt::AltModifier)&&event->key()==Qt::Key_C;

    if(!isShortCut)
        QPlainTextEdit::keyPressEvent(event);

    if(event->key()==Qt::Key_ParenLeft||
            event->key()==Qt::Key_BracketLeft||
            event->key()==Qt::Key_BraceLeft||
            event->key()==Qt::Key_QuoteDbl||
            event->key()==Qt::Key_Apostrophe){
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
    }

    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString prefix = cursor.selectedText();
    QString endSign = "~!@#$%^&*()_-+=\\/,.。；？?‘'\"{}[]<>";
    if(!isShortCut&&(endSign.contains(event->text().right(1))||prefix.length()<2)){
        mCompleter->popup()->hide();
        return;
    }

    if(mCompleter->completionPrefix()!=prefix){
      mCompleter->setCompletionPrefix(prefix);
      mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0,0));
    }
    QRect rect = cursorRect();
    rect.setWidth(mCompleter->popup()->sizeHintForColumn(0)+mCompleter->popup()->verticalScrollBar()->sizeHint().width());

    mCompleter->complete(rect);
}

void MyCodeEditor::lineNumberWidgetPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberWidget);
    //绘制行号区域
    painter.fillRect(event->rect(),QColor(100,100,100,10));

    //拿到block
    QTextBlock block = firstVisibleBlock();

    //拿到行号
    int blockNumber =block.blockNumber();

    //拿到当前的block的top
    int cursorTop = blockBoundingGeometry(textCursor().block()).translated(contentOffset()).top();

    //拿到block的top
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();

    //拿到block的bottom
    int bottom = top +blockBoundingRect(block).height();

    while(block.isValid()&&top<=event->rect().bottom()){
        //设置画笔颜色
        painter.setPen(cursorTop==top?Qt::black:Qt::gray);
        //绘制文字
        painter.drawText(0,top,getLineNumberWidgetWidth()-3,bottom-top,Qt::AlignRight,QString::number(blockNumber+1));

        //拿到下一个block
        block = block.next();

        top = bottom;
        bottom =  top +blockBoundingRect(block).height();
        blockNumber++;
    }
}

void MyCodeEditor::lineNumberWidgetMousePressEvent(QMouseEvent *event)
{
    setTextCursor(QTextCursor(document()->findBlockByLineNumber(event->y()/fontMetrics().height()+verticalScrollBar()->value())));
}

void MyCodeEditor::lineNumberWidgetWheelEvent(QWheelEvent *event)
{
    if(event->orientation()==Qt::Horizontal){
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-event->delta());
    }else {
       verticalScrollBar()->setValue(verticalScrollBar()->value()-event->delta());
    }
    event->accept();
}

bool MyCodeEditor::saveFile()
{
    QString fileName;
    if(mFileName.isEmpty()){
        fileName =QFileDialog::getSaveFileName(this,"保存文件");
        mFileName =fileName;
    }else{
        fileName =mFileName;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,"警告","无法保存文件:"+file.errorString());
        return false;
    }
    QTextStream out(&file);

    out<<toPlainText();
    file.close();

    isSaved = true;
    return  true;
}

bool MyCodeEditor::saveAsFile()
{
    QString fileName=QFileDialog::getSaveFileName(this,"另存文件");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,"警告","无法保存文件:"+file.errorString());
        return false;
    }
    mFileName =fileName;
    QTextStream out(&file);
    QString text =toPlainText();
    out<<text;
    file.close();

    isSaved = true;
    return true;
}

void MyCodeEditor::setFileName(QString fileName)
{
    mFileName=fileName;
}

QString MyCodeEditor::getFileName()
{
    return mFileName;
}


