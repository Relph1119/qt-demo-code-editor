#include "mytextedit.h"
#include "ui_mytextedit.h"
#include "QDebug"

#include <myhighlighter.h>

MyTextEdit::MyTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit)
{
    ui->setupUi(this);

    // 绑定滚动条
    initConnect();

    // 初始化字体
    initFont();

    // 初始化高亮
    initHighlighter();

    // 行高亮
    hightlightCurrentLint();
}

MyTextEdit::~MyTextEdit()
{
    delete ui;
}

void MyTextEdit::initConnect()
{
    // textChanged
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    // 滚动条
    connect(ui->textEdit->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(textEditHScrollBarChanged()));
    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarChanged()));
    connect(ui->textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(textEditVScrollBarChanged()));
    connect(ui->textBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(textBrowserVScrollBarChanged()));

    // cursor
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(hightlightCurrentLint()));
}

void MyTextEdit::initFont()
{
    mFont = QFont("Consolas", 14);
    ui->textEdit->setFont(mFont);
    // 解决行高问题
    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height() * 1.1, QTextBlockFormat::FixedHeight);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    ui->textBrowser->setFont(mFont);
}

void MyTextEdit::initHighlighter()
{
    new MyHighlighter(ui->textEdit->document());
}

void MyTextEdit::hightlightCurrentLint()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0, 100, 100, 20));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = ui->textEdit->textCursor();
//    selection.cursor.clearSelection();
    extraSelections.append(selection);

    ui->textEdit->setExtraSelections(extraSelections);
}

void MyTextEdit::textEditHScrollBarChanged()
{
    ui->horizontalScrollBar->setMinimum(ui->textEdit->horizontalScrollBar()->minimum());
    ui->horizontalScrollBar->setMaximum(ui->textEdit->horizontalScrollBar()->maximum());
    ui->horizontalScrollBar->setPageStep(ui->textEdit->horizontalScrollBar()->pageStep());
    // 将textEdit的水平滚动条的值绑定到外框的水平滚动条上
    ui->horizontalScrollBar->setValue(ui->textEdit->horizontalScrollBar()->value());
}

void MyTextEdit::scrollBarChanged()
{
    ui->textEdit->horizontalScrollBar()->setValue(ui->horizontalScrollBar->value());
}

void MyTextEdit::textEditVScrollBarChanged()
{
    ui->textBrowser->verticalScrollBar()->setMaximum(ui->textEdit->verticalScrollBar()->maximum());
    ui->textBrowser->verticalScrollBar()->setMinimum(ui->textEdit->verticalScrollBar()->minimum());
    ui->textBrowser->verticalScrollBar()->setPageStep(ui->textEdit->verticalScrollBar()->pageStep());
    ui->textBrowser->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->value());
}

void MyTextEdit::textBrowserVScrollBarChanged()
{
    ui->textEdit->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->value());
}

void MyTextEdit::onTextChanged()
{
    // 获得行号
    int lineCountOfTextEdit = ui->textEdit->document()->lineCount();
    QString text = ui->textBrowser->toPlainText();
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

    ui->textBrowser->setMaximumWidth(25 + QString::number(lineCountOfTextEdit).length()*15);
    ui->textBrowser->setText(text);

    // 解决行高问题
    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height() * 1.1, QTextBlockFormat::FixedHeight);
    format.setAlignment(Qt::AlignRight);
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}
