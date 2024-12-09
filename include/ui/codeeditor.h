#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "highlighter.h"

#include <QPlainTextEdit>
#include <QCompleter>

class LineNumberWidget;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr,QFont font=QFont("Consolas",14));

    ~CodeEditor();

    void lineNumberWidgetPaintEvent(QPaintEvent * event);
    void lineNumberWidgetMousePressEvent(QMouseEvent *event);
    void lineNumberWidgetWheelEvent(QWheelEvent *event);

    void removeLastHighlights(int lineNumber);
    void highlightLine(int lineNumber, QColor color);

    bool saveFile();
    bool saveAsFile();

    void setFileName(QString fileName);
    QString getFileName();

    void setAllFont(QFont font);

    bool checkSaved();

private slots:
    void highlightCurrentLine();
    void updateLineNumberWidget(QRect rect, int dy);
    void updateLineNumberWidgetWidth();
    void updateSaveState();

    void insertCompletion(const QString &completion);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent * event) override;

private:
    void initConnection();
    void initHighlighter();
    void initCompleter();
    int getLineNumberWidgetWidth();


    LineNumberWidget * lineNumberWidget;
    Highlighter * mHighlighter;

    QString mFileName;
    bool isSaved = false;
    QCompleter * mCompleter;

signals:

};

class LineNumberWidget :public QWidget{
public:
    explicit LineNumberWidget(CodeEditor *editor=nullptr):QWidget(editor){
        codeEditor=editor;
    }

protected:
    void  paintEvent(QPaintEvent *event) override{
        //把绘制任务提交给CodeEditor
        codeEditor->lineNumberWidgetPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event) override{
        //把鼠标点击任务提交给CodeEditor
        codeEditor->lineNumberWidgetMousePressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override{
        //把滚轮任务提交给CodeEditor
        codeEditor->lineNumberWidgetWheelEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CodeEditor_H
