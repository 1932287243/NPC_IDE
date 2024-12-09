#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class Highlighter: public QSyntaxHighlighter
{
public:
    explicit Highlighter(QTextDocument *parent = nullptr,QString fontFamily="Consolas",int fontSize=14);
    void setFont(QFont font);
protected:
    void highlightBlock(const QString &text) ;

private:

    QString mFontFamily ;
    int mFontSize ;

    struct HighlightRule{
      QRegExp pattern;
      QTextCharFormat format;
    };

    QVector<HighlightRule> highlightRules;

    void initFormat();

    void addNormalTextFormat();
    void addNumberFormat();
    void addStringFormat();
    void addCommentFormat();

    void addMultiLineCommentFormat(const QString &text);

    void addKeywordsFromat();
    void addClassNameFormat();
    void addFunctionFormat();
};

#endif // Highlighter_H
