#include "mycodeeditor.h"
#include "myhighlighter.h"

#include <QPainter>
#include <QRect>
#include <QChar>
#include <QScrollBar>

MyCodeEditor::MyCodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberWidget = new LineNumberWidget(this);

    // 初始字体
    initFont();

    // 绑定
    initConnection();

    // 高亮
    initHighlighter();

    // 当前行高亮
    hightlightCurrentLint();

    // 设置边距
    updateLineNumberWidgetWith();

    setLineWrapMode(QPlainTextEdit::NoWrap);
}

void MyCodeEditor::initFont()
{
    this->setFont(QFont("Consolas", 14));
}

void MyCodeEditor::initHighlighter()
{
    new MyHighlighter(document());
}

int MyCodeEditor::getLineNumberWidgetWidth()
{
    // 获取自适应的宽度
    return 8 + QString::number(blockCount() + 1).length() * fontMetrics().horizontalAdvance(QChar('0'));
}

void MyCodeEditor::initConnection()
{
    // cursor
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(hightlightCurrentLint()));

    // blockCount
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidgetWith()));

    // updateRequest
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberWidget(QRect, int)));
}

void MyCodeEditor::hightlightCurrentLint()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0, 100, 100, 20));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();

    extraSelections.append(selection);

    this->setExtraSelections(extraSelections);
}

void MyCodeEditor::updateLineNumberWidget(QRect rect, int dy)
{
    if (dy) {
        lineNumberWidget->scroll(0, dy);
    } else {
        lineNumberWidget->update(0, rect.y(), getLineNumberWidgetWidth(), rect.height());
    }
}

void MyCodeEditor::updateLineNumberWidgetWith()
{
    // 设置边距
    setViewportMargins(getLineNumberWidgetWidth(), 0, 0, 0);
}

void MyCodeEditor::resizeEvent(QResizeEvent *event)
{
    // 目的是为了窗口自适应
    QPlainTextEdit::resizeEvent(event);
    lineNumberWidget->setGeometry(0, 0, getLineNumberWidgetWidth(), contentsRect().height());
}

void MyCodeEditor::lineNumberWidgetPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberWidget);
    // 绘制行号区域
    painter.fillRect(event->rect(), QColor(100, 100, 100, 20));

    // 获取block
    QTextBlock block = firstVisibleBlock();

    // 获取行号
    int blockNumber = block.blockNumber();

    // 获取当前的block的top
    int cursorTop = blockBoundingGeometry(textCursor().block()).translated(contentOffset()).top();

    // 获取block的top
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();

    // 获取block的bottom
    int bottom = top + blockBoundingRect(block).height();

    while(block.isValid() && top <= event->rect().bottom()) {
        // 设置画笔颜色
        painter.setPen(cursorTop==top?Qt::black:Qt::gray);

        // 绘制文字
        painter.drawText(0, top, getLineNumberWidgetWidth() - 3, bottom - top, Qt::AlignRight, QString::number(blockNumber + 1));

        // 遍历下一个block
        block = block.next();

        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        blockNumber++;
    }

}

void MyCodeEditor::lineNumberWidgetMousePressEvent(QMouseEvent *event)
{
    // 加上隐藏的距离
    QTextBlock block = document()->findBlockByLineNumber(event->y() / fontMetrics().height() + verticalScrollBar()->value());
    setTextCursor(QTextCursor(block));
}

void MyCodeEditor::lineNumberWidgetWheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Horizontal) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->delta());
    } else {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - event->delta());
    }
    event->accept();
}

