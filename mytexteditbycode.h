#ifndef MYTEXTEDITBYCODE_H
#define MYTEXTEDITBYCODE_H

#include <QScrollBar>
#include <QTextBrowser>
#include <QTextEdit>
#include <QWidget>

class MyTextEditByCode: public QWidget
{
    Q_OBJECT
public:
    explicit MyTextEditByCode(QWidget *parent = nullptr);
private slots:
    void onScrollBarChanged();
    void onTextEditHorizontalScrollBarChanged();
    void onTextEditVerticalScrollBarChanged();
    void onTextBrowserVerticalScrollBarChanged();
    void onTextChanged();
    void hightlightCurrentLint();
private:
    QTextEdit *textEdit;
    QTextBrowser *textBrowser;
    QScrollBar *scrollBar;
    QFont mFont;

    void initWidget();
    void initFont();
    void initConnect();
    void initHighlighter();
signals:

};

#endif // MYTEXTEDITBYCODE_H
