#include "mycodeeditor.h"
#include "myhighlighter.h"

#include <QPainter>
#include <QRect>
#include <QChar>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MyCodeEditor::MyCodeEditor(QWidget *parent, QFont font) : QPlainTextEdit(parent)
{
    lineNumberWidget = new LineNumberWidget(this);

    // 绑定
    initConnection();

    // 高亮
    initHighlighter();

    // 设置字体
    setAllFont(font);

    // 当前行高亮
    hightlightCurrentLint();

    // 设置边距
    updateLineNumberWidgetWidth();

    setLineWrapMode(QPlainTextEdit::NoWrap);
}

MyCodeEditor::~MyCodeEditor()
{
    delete lineNumberWidget;
}

void MyCodeEditor::initConnection()
{
    // cursor
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(hightlightCurrentLint()));

    // textChanged
    connect(this, SIGNAL(textChanged()), this, SLOT(updateSaveState()));

    // blockCount
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidgetWidth()));

    // updateRequest
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberWidget(QRect,int)));
}

void MyCodeEditor::setAllFont(QFont font)
{
    this->setFont(font);
    myHighlighter->setFont(font);
    updateLineNumberWidgetWidth();
}

bool MyCodeEditor::checkSaved()
{
    return isSaved;
}

void MyCodeEditor::initHighlighter()
{
    myHighlighter = new MyHighlighter(document());
}

int MyCodeEditor::getLineNumberWidgetWidth()
{
    // 获取自适应的宽度
    return 8 + QString::number(blockCount() + 1).length() * fontMetrics().horizontalAdvance(QChar('0'));
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

void MyCodeEditor::updateLineNumberWidgetWidth()
{
    // 设置边距
    setViewportMargins(getLineNumberWidgetWidth(), 0, 0, 0);
}

void MyCodeEditor::updateSaveState()
{
    // 更新保存状态
    isSaved = false;
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

bool MyCodeEditor::saveFile()
{
    QString fileName;
    if (mFileName.isEmpty()) {
        // 提示用户输入文件名，并获取保存文件的文件名
        fileName = QFileDialog::getSaveFileName(this, "保存文件");
        mFileName = fileName;
    } else {
        fileName = mFileName;
    }
    QFile file(fileName);
    // 处理文件保存异常
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "无法保存文件，报错信息：\n" + file.errorString());
        return false;
    }
    // 保存文件内容
    QTextStream out(&file);

    out << toPlainText();
    file.close();

    isSaved = true;
    return true;
}

bool MyCodeEditor::saveAsFile()
{
    // 获取另存为的文件名
    QString fileName = QFileDialog::getSaveFileName(this, "另存文件");

    QFile file(fileName);
    // 处理另存为文件异常
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "无法保存文件，报错信息：\n" + file.errorString());
        return false;
    }
    mFileName = fileName;

    // 保存文件内容
    QTextStream out(&file);
    QString text = toPlainText();
    out << text;
    file.close();

    isSaved = true;
    return true;
}

void MyCodeEditor::setFileName(QString fileName)
{
    mFileName = fileName;
}

QString MyCodeEditor::getFileName()
{
    return mFileName;
}



