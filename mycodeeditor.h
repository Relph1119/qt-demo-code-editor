#ifndef MYCODEEDITOR_H
#define MYCODEEDITOR_H

#include "myhighlighter.h"

#include <QPlainTextEdit>
#include <QWidget>

class LineNumberWidget;

class MyCodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MyCodeEditor(QWidget *parent = nullptr, QFont font=QFont("Consolas", 14));
    ~MyCodeEditor();

    void lineNumberWidgetPaintEvent(QPaintEvent *event);
    void lineNumberWidgetMousePressEvent(QMouseEvent *event);
    void lineNumberWidgetWheelEvent(QWheelEvent *event);

    bool saveFile();
    bool saveAsFile();
    void setFileName(QString fileName);
    QString getFileName();

    void setAllFont(QFont font);

    bool checkSaved();

private slots:
    void hightlightCurrentLint();
    void updateLineNumberWidget(QRect rect, int dy);
    void updateLineNumberWidgetWidth();
    void updateSaveState();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    void initConnection();
    void initHighlighter();
    int getLineNumberWidgetWidth();

    LineNumberWidget * lineNumberWidget;

    MyHighlighter *myHighlighter;

    QString mFileName;

    bool isSaved = false;
signals:

};

class LineNumberWidget : public QWidget {
public:
    explicit LineNumberWidget(MyCodeEditor *editor=nullptr): QWidget(editor) {
        codeEditor = editor;
    };
protected:
    void paintEvent(QPaintEvent *event) override {
        // 把绘制任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetPaintEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override {
        // 把鼠标点击任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetMousePressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override {
        // 把鼠标滚轮任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetWheelEvent(event);
    }
private:
    MyCodeEditor * codeEditor;
};

#endif // MYCODEEDITOR_H
