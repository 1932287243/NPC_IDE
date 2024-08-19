#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QWidget>

namespace Ui {
class MyTextEdit;
}

class MyTextEdit : public QWidget
{
    Q_OBJECT

public:
    explicit MyTextEdit(QWidget *parent = nullptr);
    ~MyTextEdit();

private slots:
    void textEditHScrollBarChanged();
    void scrollBarChanged();

    void textEditVScrollBarChanged();
    void textBrowserVScrollBarChanged();

    void onTextChanged();
    void highlightCurrentLine();

private:
    Ui::MyTextEdit *ui;
    void initConnect();
    void initFont();
    void initHighlighter();

    QFont mFont;
};

#endif // MYTEXTEDIT_H
