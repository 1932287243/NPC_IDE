#include "codeeditor.h"
#include "highlighter.h"

#include <QPainter>
#include <QTextStream>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QAbstractItemView>

#include<QDebug>

CodeEditor::CodeEditor(QWidget *parent,QFont font)
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

CodeEditor::~CodeEditor()
{
    // qDebug() << "CodeEditor" ;
    delete  lineNumberWidget;
}

void CodeEditor::initConnection()
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

void CodeEditor::setAllFont(QFont font)
{
    this->setFont(font);
    mHighlighter->setFont(font);
    mHighlighter->rehighlight();
    mCompleter->popup()->setFont(font);
    viewport()->update();
    lineNumberWidget->update();
    updateLineNumberWidgetWidth();
}

bool CodeEditor::checkSaved()
{
    return isSaved;
}

void CodeEditor::initHighlighter()
{
    mHighlighter =  new Highlighter(document());
}

void CodeEditor::initCompleter()
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

int CodeEditor::getLineNumberWidgetWidth()
{
    //获取宽度（合适）
    return 8+QString::number(blockCount()+1).length()*fontMetrics().horizontalAdvance(QChar('0'));
}


void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor =textCursor();

    extraSelections.append(selection);
    setExtraSelections(extraSelections);

}

void CodeEditor::updateLineNumberWidget(QRect rect, int dy)
{
    if(dy)
        lineNumberWidget->scroll(0,dy);
    else
        lineNumberWidget->update(0,rect.y(),getLineNumberWidgetWidth(),rect.height());
}

void CodeEditor::updateLineNumberWidgetWidth()
{
    //设置边距
    setViewportMargins(getLineNumberWidgetWidth(),0,0,0);
}

void CodeEditor::updateSaveState()
{
    //更新保存状态
    isSaved = false;
}

void CodeEditor::insertCompletion(const QString &completion)
{
    QTextCursor cursor = textCursor();



    int length =completion.length()-mCompleter->completionPrefix().length();

    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);

    cursor.insertText(completion.right(length));

    setTextCursor(cursor);
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    lineNumberWidget->setGeometry(0,0,getLineNumberWidgetWidth(),contentsRect().height());
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
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

void CodeEditor::lineNumberWidgetPaintEvent(QPaintEvent *event)
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

void CodeEditor::lineNumberWidgetMousePressEvent(QMouseEvent *event)
{
    setTextCursor(QTextCursor(document()->findBlockByLineNumber(event->y()/fontMetrics().height()+verticalScrollBar()->value())));
}

void CodeEditor::lineNumberWidgetWheelEvent(QWheelEvent *event)
{
    if(event->orientation()==Qt::Horizontal){
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-event->delta());
    }else {
       verticalScrollBar()->setValue(verticalScrollBar()->value()-event->delta());
    }
    event->accept();
}

bool CodeEditor::saveFile()
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

bool CodeEditor::saveAsFile()
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

void CodeEditor::setFileName(QString fileName)
{
    mFileName=fileName;
}

QString CodeEditor::getFileName()
{
    return mFileName;
}

void CodeEditor::highlightLine(int lineNumber, QColor color) 
{
    // QTextCursor cursor = this->textCursor();
    
    // // 移动到指定行
    // // QTextBlock block = this->document()->findBlockByNumber(lineNumber - 1); // lineNumber is 1-based
    // cursor.setPosition(this->document()->findBlockByNumber(lineNumber).position());
    // this->setTextCursor(cursor);
    // this->ensureCursorVisible();
    // // // 设置光标到该行的起始位置
    // // cursor.setPosition(block.position());
    // // cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    // // 设置该行的背景色
    // QTextCharFormat format;
    // format.setBackground(color);
    
    // // 应用格式
    // cursor.mergeCharFormat(format);


    // 获取指定行的 QTextBlock
    QTextBlock block = this->document()->findBlockByNumber(lineNumber);
    if (!block.isValid()) {
        qDebug() << "Invalid block at line" << lineNumber;
        return;
    }

    // 设置光标位置到该行的起始位置
    QTextCursor cursor(block);
    this->setTextCursor(cursor);
    
    // 设置整行的背景颜色
    QTextBlockFormat blockFormat;
    blockFormat.setBackground(color);  // 设置背景颜色

    // 设置当前行的格式
    cursor.mergeBlockFormat(blockFormat);

    // 确保光标所在行可见
    this->ensureCursorVisible();
}

// 取消所有行的高亮
void CodeEditor::removeLastHighlights(int lineNumber) 
{ 
    // 获取指定行的 QTextBlock
    QTextBlock block = this->document()->findBlockByNumber(lineNumber);
    if (!block.isValid()) {
        qDebug() << "Invalid block at line" << lineNumber;
        return;
    }

    // 设置光标位置到该行的起始位置
    QTextCursor cursor(block);
    this->setTextCursor(cursor);

    // 恢复默认背景颜色，即清除背景颜色
    QTextBlockFormat blockFormat;
    blockFormat.setBackground(Qt::transparent);  // 设置背景为透明，恢复默认背景

    // 应用格式
    cursor.mergeBlockFormat(blockFormat);

    // 确保光标所在行可见
    this->ensureCursorVisible();
}