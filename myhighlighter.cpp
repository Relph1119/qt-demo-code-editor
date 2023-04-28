#include "myhighlighter.h"

#include <QTextStream>


MyHighlighter::MyHighlighter(QTextDocument *parent, QString fontFamily, int fontSize): QSyntaxHighlighter(parent)
{
    mFontFamily = fontFamily;
    mFontSize = fontSize;

    // 对于普通文本
    addNormalTextFormat();

    // 对于数字
    addNumberFormat();

    // 对于字符串
    addStringFormat();

    // 对于注释
    addCommentFormat();

    // 对于关键字/保留字
    addKeywordsFormat();

    // 对于类名
    addClassNameFormat();

    // 对于方法/函数
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

    // 设置高亮格式
    normalTextFormat.setFont(QFont(mFontFamily, mFontSize));
    normalTextFormat.setForeground(QColor(0, 0, 0));

    rule.format = normalTextFormat;

    highlightRules.append(rule);
}

void MyHighlighter::addNumberFormat()
{
    HighlightRule rule;
    rule.pattern = QRegExp("\\b\\d+|\\d+\\.\\d+\\b");

    QTextCharFormat numberFormat;

    // 设置高亮格式
    numberFormat.setFont(QFont(mFontFamily, mFontSize));
    numberFormat.setForeground(QColor(250, 80, 50));

    rule.format = numberFormat;

    highlightRules.append(rule);
}

void MyHighlighter::addStringFormat()
{
    QTextCharFormat stringFormat;

    // 设置高亮格式
    stringFormat.setFont(QFont(mFontFamily, mFontSize));
    stringFormat.setForeground(QColor(0, 180, 180));

    HighlightRule rule;
    rule.format = stringFormat;

    // 匹配''
    rule.pattern = QRegExp("'[^']*'");
    highlightRules.append(rule);

    // 匹配""
    rule.pattern = QRegExp("\"[^\"]*\"");
    highlightRules.append(rule);

    // 匹配``
    rule.pattern = QRegExp("`[^`]*`");
    highlightRules.append(rule);
}

void MyHighlighter::addCommentFormat()
{
    QTextCharFormat commentFormat;

    // 设置高亮格式
    commentFormat.setFont(QFont(mFontFamily, mFontSize));
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);

    HighlightRule rule;
    rule.format = commentFormat;

    // 匹配 //dada
    rule.pattern = QRegExp("\\/\\/.*$");
    highlightRules.append(rule);
}

void MyHighlighter::addMultiCommentFormat(const QString &text)
{
    setCurrentBlockState(0);

    // 匹配 /*
    QRegExp commentStartRegExp("\\/\\*");

    // 匹配 */
    QRegExp commentEndRegExp("\\*\\/");

    // 高亮格式
    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setFont(QFont(mFontFamily, mFontSize));
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    multiLineCommentFormat.setFontItalic(true);

    int startIndex = 0;
    // 判断前一行
    if (previousBlockState() != 1) {
        startIndex = commentStartRegExp.indexIn(text);
    }

    while (startIndex >= 0) {
        int endIndex = commentEndRegExp.indexIn(text, startIndex);
        int commentLength = 0;
        if (endIndex == -1) { // 没有结束位置
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else { // 有结束位置
            commentLength = endIndex - startIndex + commentEndRegExp.matchedLength();
        }
        // 设置格式
        setFormat(startIndex,
                  commentLength,
                  multiLineCommentFormat);
        startIndex = commentStartRegExp.indexIn(text, commentLength + startIndex);
    }
}

void MyHighlighter::addKeywordsFormat()
{
    QFile file(":/config/config/keywords.txt");
    QTextStream keywordsStream(&file);

    HighlightRule rule;
    QTextCharFormat keywordsFormat;
    keywordsFormat.setFont(QFont(mFontFamily, mFontSize));
    keywordsFormat.setForeground(Qt::darkMagenta);
    rule.format = keywordsFormat;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        keywordsStream.seek(0);
        QString line;
        while (!keywordsStream.atEnd()) {
            line = keywordsStream.readLine();
            if (!line.isEmpty()) {
                rule.pattern = QRegExp("\\b" + line + "\\b");
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

    // 设置高亮格式
    classNameFormat.setFont(QFont(mFontFamily, mFontSize));
    classNameFormat.setForeground(QColor(150, 20, 100));
    classNameFormat.setFontWeight(99);

    rule.format = classNameFormat;

    rule.pattern = QRegExp("\\b[A-Z]+\\w*");
    highlightRules.append(rule);
}

void MyHighlighter::addFunctionFormat()
{
    HighlightRule rule;

    QTextCharFormat functionFormat;

    // 设置高亮格式
    functionFormat.setFont(QFont(mFontFamily, mFontSize));
    functionFormat.setForeground(QColor(200, 0, 150));

    rule.format = functionFormat;

    rule.pattern = QRegExp("\\w+\\(");
    highlightRules.append(rule);
    rule.pattern = QRegExp("\\)");
    highlightRules.append(rule);
}

// 按行传入
void MyHighlighter::highlightBlock(const QString &text)
{
    foreach(const HighlightRule &rule, highlightRules) {
        QRegExp regExp(rule.pattern);
        int index = regExp.indexIn(text);
        while (index >= 0) {
            int length = regExp.matchedLength();
            setFormat(index, length, rule.format);
            // 增加遍历长度
            index = regExp.indexIn(text, index + length);
        }
    }

    // 对于多行注释
    addMultiCommentFormat(text);
}
