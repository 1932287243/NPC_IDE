#include "myhighlighter.h"

#include <QDebug>


MyHighlighter::MyHighlighter(QTextDocument *parent, QString fontFamily, int fontSize): QSyntaxHighlighter(parent)
{
    mFontFamily =fontFamily;
    mFontSize = fontSize;


    initFormat();

}

void MyHighlighter::initFormat()
{
    //对于普通文本
    addNormalTextFormat();

    //对于数字
    addNumberFormat();

    //对于字符串
    addStringFormat();

    //对于注释
    addCommentFormat();

    //对于关键字/保留字
    addKeywordsFromat();

    //对于类名
    addClassNameFormat();

    //对于方法/函数
    addFunctionFormat();
}
void MyHighlighter::setFont(QFont font)
{
    mFontFamily = font.family();
    mFontSize = font.pointSize();
}


void MyHighlighter::addNormalTextFormat()
{
    HighlightRule rule;
    rule.pattern = QRegExp("[a-z0-9A-Z]+");

    QTextCharFormat normalTextFormat;

    normalTextFormat.setForeground(QColor(0,0,0));

    rule.format = normalTextFormat;

    highlightRules.append(rule);
}

void MyHighlighter::addNumberFormat()
{
    HighlightRule rule;
    rule.pattern = QRegExp("\\b\\d+|\\d+\\.\\d+\\b");

    QTextCharFormat numberFormat;

    numberFormat.setForeground(QColor(250,80,50));

    rule.format = numberFormat;

    highlightRules.append(rule);
}

void MyHighlighter::addStringFormat()
{
    QTextCharFormat stringFormat;

    stringFormat.setForeground(QColor(0,180,180));


    HighlightRule rule;

    rule.format = stringFormat;

    //''
    rule.pattern = QRegExp("'[^']*'");
    highlightRules.append(rule);

    //""
    rule.pattern = QRegExp("\"[^\"]*\"");
    highlightRules.append(rule);

    //``
    rule.pattern = QRegExp("`[^`]*`");
    highlightRules.append(rule);

}

void MyHighlighter::addCommentFormat()
{
    QTextCharFormat commnetFormat;

    commnetFormat.setForeground(Qt::darkGreen);
    commnetFormat.setFontItalic(true);


    HighlightRule rule;

    rule.format = commnetFormat;

    // //dada
    rule.pattern = QRegExp("\\/\\/.*$");
    highlightRules.append(rule);
}

void MyHighlighter::addMultiLineCommentFormat(const QString &text)
{
    setCurrentBlockState(0);

    // /*
    QRegExp commentStartRegExp("/\\*");
    // */
    QRegExp commentEndRegExp("\\*/");

    //高亮格式
    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    multiLineCommentFormat.setFontItalic(true);

    int startIndex = 0;
    if(previousBlockState()!=1)
       startIndex= commentStartRegExp.indexIn(text);

    while(startIndex>=0){
        int endIndex = commentEndRegExp.indexIn(text,startIndex);
        int commentLength = 0;
        if(endIndex==-1){//没有结束位置
            setCurrentBlockState(1);
            commentLength = text.length()-startIndex;
        }else{//有结束位置
            commentLength = endIndex-startIndex+commentEndRegExp.matchedLength();
        }
        //设置格式
        setFormat(startIndex,
                  commentLength,
                  multiLineCommentFormat);
        startIndex = commentStartRegExp.indexIn(text,commentLength+startIndex);
    }
}

void MyHighlighter::addKeywordsFromat()
{
    QFile file(":/config/config/keywords.txt");
    QTextStream keywordsStream(&file);

    HighlightRule rule ;
    QTextCharFormat keywordsFormat;
    keywordsFormat.setForeground(Qt::darkYellow);
    rule.format = keywordsFormat;

    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        keywordsStream.seek(0);
        QString line ;
        while (!keywordsStream.atEnd()) {
           line = keywordsStream.readLine();
           if(line!=""){
               rule.pattern = QRegExp("\\b"+line+"\\b");
               highlightRules.append(rule);
           }
        }
        file.close();
    }
}

void MyHighlighter::addClassNameFormat()
{
    HighlightRule rule;

    QTextCharFormat classNameFormat;
    classNameFormat.setForeground(QColor(150,20,100));
    classNameFormat.setFontWeight(99);

    rule.format = classNameFormat;

    rule.pattern = QRegExp("\\b[A-Z]+\\w*");
    highlightRules.append(rule);
}

void MyHighlighter::addFunctionFormat()
{
    HighlightRule rule;

    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(200,0,150));

    rule.format = functionFormat;

    rule.pattern = QRegExp("\\w+\\(");
    highlightRules.append(rule);
    rule.pattern = QRegExp("\\)");
    highlightRules.append(rule);
}



//按行来传入的
void MyHighlighter::highlightBlock(const QString &text)
{
    QRegExp regExp;
    foreach (const HighlightRule &rule, highlightRules) {
        regExp=rule.pattern;
        int index = regExp.indexIn(text);
        while(index>=0){
            int length =regExp.matchedLength();
            QTextCharFormat format =  rule.format;
            format.setFont(QFont(mFontFamily,mFontSize));
            setFormat(index,length,format);
            index = regExp.indexIn(text,index+length);
        }
    }

    //对于多行注释
    addMultiLineCommentFormat(text);
}


