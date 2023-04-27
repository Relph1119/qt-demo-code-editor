#include "mytexteditbycode.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <myhighlighter.h>
#include <QFontMetrics>
#include <QFont>
#include <QList>

MyTextEditByCode::MyTextEditByCode(QWidget *parent): QWidget(parent)
{
    // UI组件
    initWidget();

    // 字体
    initFont();

    // 绑定
    initConnect();

    // 高亮
    initHighlighter();

    // 当前行高亮
    hightlightCurrentLint();
}


void MyTextEditByCode::initWidget()
{
    QWidget * horizontalLayoutWidget = new QWidget();
    // 竖向布局
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    verticalLayout->setSpacing(0);
    verticalLayout->setMargin(0);

    // 横向布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setMargin(0);

    // 设置文本框
    textEdit = new QTextEdit();
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);

    // 设置代码行框
    textBrowser = new QTextBrowser();
    textBrowser->setMaximumWidth(25);
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 设置滚动条
    scrollBar = new QScrollBar();
    scrollBar->setOrientation(Qt::Horizontal);

    // 添加组件
    horizontalLayout->addWidget(textBrowser);
    horizontalLayout->addWidget(textEdit);
    verticalLayout->addWidget(horizontalLayoutWidget);
    verticalLayout->addWidget(scrollBar);
}

void MyTextEditByCode::initFont()
{
    mFont = QFont("Consolas", 14);
    textEdit->setFont(mFont);
    // 解决行高问题
    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height() * 1.1, QTextBlockFormat::FixedHeight);

    QTextCursor cursor = textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);

    textBrowser->setFont(mFont);
}

void MyTextEditByCode::initConnect()
{
    // textChanged
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    // 滚动条
    connect(scrollBar, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarChanged()));
    connect(textEdit->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onTextEditHorizontalScrollBarChanged()));
    connect(textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onTextEditVerticalScrollBarChanged()));
    connect(textBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onTextBrowserVerticalScrollBarChanged()));

    // cursor
    connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(hightlightCurrentLint()));
}

void MyTextEditByCode::initHighlighter()
{
    new MyHighlighter(textEdit->document());
}

void MyTextEditByCode::hightlightCurrentLint()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0, 100, 100, 20));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textEdit->textCursor();
//    selection.cursor.clearSelection();
    extraSelections.append(selection);

    textEdit->setExtraSelections(extraSelections);
}

void MyTextEditByCode::onScrollBarChanged()
{
    textEdit->horizontalScrollBar()->setValue(scrollBar->value());
}

void MyTextEditByCode::onTextEditHorizontalScrollBarChanged()
{
    scrollBar->setMaximum(textEdit->horizontalScrollBar()->maximum());
    scrollBar->setMinimum(textEdit->horizontalScrollBar()->minimum());
    scrollBar->setPageStep(textEdit->horizontalScrollBar()->pageStep());
    scrollBar->setValue(textEdit->horizontalScrollBar()->value());
}

void MyTextEditByCode::onTextEditVerticalScrollBarChanged()
{
    textBrowser->verticalScrollBar()->setMaximum(textEdit->verticalScrollBar()->maximum());
    textBrowser->verticalScrollBar()->setMinimum(textEdit->verticalScrollBar()->minimum());
    textBrowser->verticalScrollBar()->setPageStep(textEdit->verticalScrollBar()->pageStep());
    textBrowser->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value());
}

void MyTextEditByCode::onTextBrowserVerticalScrollBarChanged()
{
    textEdit->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->value());
}

void MyTextEditByCode::onTextChanged()
{
    // 获取行号
    int lineCountOfTextEdit = textEdit->document()->lineCount();

    QString text = textBrowser->toPlainText();
    int lineCountOfTextBrowser = text.trimmed().split("\n").length();

    if (lineCountOfTextBrowser > lineCountOfTextEdit) {
        for (int i = lineCountOfTextBrowser; i > lineCountOfTextEdit; i--) {
            text.chop((QString::number(i) + "\n").length());
        }
    } else if (lineCountOfTextBrowser == 1 && text.length() < 1) {
        text += "1\n";
    } else if (lineCountOfTextBrowser < lineCountOfTextEdit){
        for (int i = lineCountOfTextBrowser; i < lineCountOfTextEdit; i++) {
            text += QString::number(i + 1) + "\n";
        }
    }

    textBrowser->setMaximumWidth(25 + QString::number(lineCountOfTextEdit).length()*15);
    textBrowser->setText(text);

    // 解决行高问题
    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height() * 1.1, QTextBlockFormat::FixedHeight);
    format.setAlignment(Qt::AlignRight);
    QTextCursor cursor = textBrowser->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}
