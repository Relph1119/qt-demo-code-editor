#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QSettings>
#include <QTextStream>
#include <QFont>

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#endif
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置文本框居中
    this->setCentralWidget(ui->tabWidget);

    mSettings = new QSettings("settings.ini", QSettings::IniFormat);

    // 初始化菜单
    initMenu();

    // 初始化字体
    initFont();

    // 初始化动作
    initAction();

#if !QT_CONFIG(printer)
    ui->print->setEnabled(false);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFont()
{
    mfontFamily = mSettings->value("font_family", "Consolas").toString();
    mfontSize = mSettings->value("font_size", 14).toInt();
}

void MainWindow::initAction()
{
    bool vaild = ui->tabWidget->count() > 0;
    ui->save_file->setEnabled(vaild);
    ui->save_as->setEnabled(vaild);
    ui->copy->setEnabled(vaild);
    ui->paste->setEnabled(vaild);
    ui->cut->setEnabled(vaild);
    ui->undo->setEnabled(vaild);
    ui->redo->setEnabled(vaild);
}

// 初始化最近打开的文件列表
void MainWindow::initMenu()
{
    // 获取Menu
    QMenu *recent = this->findChild<QMenu *>("recent");
    // 获取Action
    QSet<QObject *> chList = recent->children().toSet();
    foreach (QObject *ch, chList) {
        QAction *action = (QAction *) ch;
        // 清空子菜单Action
        recent->removeAction(action);
    }

    QList<QString> list = getHistory();

    // 设置菜单的执行事件，将最近打开的文件放在最前面
    for(int i = list.size() - 1; i >= 0; i--) {
        // 生成子菜单Action
        recent->addAction(list[i], this, &MainWindow::on_open_recent_file);
    }

    // 添加清除历史记录
    if(list.size() > 0) {
        recent->addAction("清除历史记录", this, &MainWindow::on_clear_history_triggered, QKeySequence("Ctrl+Alt+Shift+C"));
    }
}


// 获取历史记录
QList<QString> MainWindow::getHistory() {
    // 打开开始读，获取数组长度
    int size = mSettings->beginReadArray("history");

    // 创建返回对象
    QList<QString> list;
    for(int i = 0; i < size; i++) {
        mSettings->setArrayIndex(i);
        // 读取文件路径
        QString path = mSettings->value("path").toString();
        list.append(path);
    }

    // 关闭读
    mSettings->endArray();
    return list;
}

// 保存打开文件的历史记录
void MainWindow::saveHistory(QString path) {
    // 获取历史记录
    QList<QString> list = getHistory();

    // 去重，不能使用list = list.toSet().toList()，会导致顺序乱了
    foreach(QString str, list) {
        if(str == path) {
            list.removeOne(str);
        }
    }
    list.append(path);

    mSettings->endArray();

    // 打开开始写入
    mSettings->beginWriteArray("history");

    for(int i = 0; i < list.size(); i++) {
        // 将该配置放到最后一个
        mSettings->setArrayIndex(i);
        // 保存字符串
        mSettings->setValue("path", list[i]);
    }

    // 关闭开始写入，需要与beginWriteArray成对出现
    mSettings->endArray();
}

// 新建文件
void MainWindow::on_new_file_triggered()
{
    // 使用QPlainTextEdit
    MyCodeEditor * myCodeEditor = new MyCodeEditor(this, QFont(mfontFamily, mfontSize));
    ui->tabWidget->addTab(myCodeEditor, "NewFile.txt");
    initAction();
}

// 打开最近打开的文件
void MainWindow::on_open_recent_file()
{
    // 得到当前点击的事件
    QAction *action = (QAction *) sender();
    // 设置当前事件指向的文件名
    QString fileName = action->text();

    createTab(fileName);
}


// 打开文件
void MainWindow::on_open_file_triggered()
{
    // 获得打开文件弹出框的文件名称
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件");
    createTab(fileName);
}

// 创建Tab
void MainWindow::createTab(QString fileName)
{
    // 打开文件
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "无法打开此文件，报错信息：\n" + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();

    // 创建对象
    MyCodeEditor *codeEditor = new MyCodeEditor(this, QFont(mfontFamily, mfontSize));
    codeEditor->setPlainText(text);

    // 设置文件名
    codeEditor->setFileName(fileName);
    // 添加tab
    ui->tabWidget->addTab(codeEditor, fileName);
    // 设置当前文件的索引
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

    file.close();
    saveHistory(fileName);
    initMenu();
    initAction();
}

// 保存文件
void MainWindow::on_save_file_triggered()
{
    // 将保存交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();

    if (codeEditor) {
        if(codeEditor->saveFile()) {
            saveSuccessAction(codeEditor);
        }
    }
}

// 另存为
void MainWindow::on_save_as_triggered()
{
    // 将另存为交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();

    if (codeEditor) {
        if(codeEditor->saveAsFile()) {
            saveSuccessAction(codeEditor);
        }
    }
}

// 粘贴
void MainWindow::on_paste_triggered()
{
    // 将粘贴交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if (codeEditor) {
        codeEditor->paste();
    }
}

// 剪切
void MainWindow::on_cut_triggered()
{
    // 将剪切交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if (codeEditor) {
        codeEditor->cut();
    }
}

// 复制
void MainWindow::on_copy_triggered()
{
    // 将复制交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if (codeEditor) {
        codeEditor->copy();
    }
}

// 字体
void MainWindow::on_font_triggered()
{
    bool fontSelected;
    // 打开字体对话框
    QFont font = QFontDialog::getFont(&fontSelected, QFont(mfontFamily, mfontSize), this);
    if (fontSelected) {
        // 将复制交给CodeEditor
        MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
        if (codeEditor) {
            codeEditor->setAllFont(font);
        }
        mSettings->setValue("font_family", font.family());
        mSettings->setValue("font_size", font.pointSize());
    }
}

// 关于
void MainWindow::on_about_triggered()
{
    QMessageBox::about(this, "这是我的Nodepad!", "这是我的Nodepad，欢迎学习和使用！");
}

// 撤销
void MainWindow::on_undo_triggered()
{
    // 将撤销交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if (codeEditor) {
        codeEditor->undo();
    }
}

// 取消撤销
void MainWindow::on_redo_triggered()
{
    // 将取消撤销交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if (codeEditor) {
        codeEditor->redo();
    }
}

// 退出
void MainWindow::on_exit_triggered()
{
    QCoreApplication::exit();
}

// 打印
void MainWindow::on_print_triggered()
{
    // 把打印交给CodeEditor
    MyCodeEditor *codeEditor  = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if (codeEditor) {
#if defined (QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
        // 获得打印设备
        QPrinter printDev;
#if QT_CONFIG(printdialog)
        // 打开打印窗口
        QPrintDialog dialog(&printDev, this);
        if(dialog.exec() == QDialog::Rejected) {
            return;
        }
#endif
        // 将文本框中的内容进行打印
        codeEditor->print(&printDev);
#endif
    }
}

void MainWindow::on_clear_history_triggered()
{
    mSettings->remove("history");
    initMenu();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    MyCodeEditor * codeEditor = (MyCodeEditor *) ui->tabWidget->currentWidget();

    if(!codeEditor->checkSaved()) {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "警告", "您还没有保存文档！是否保存？",
                                                                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (btn == QMessageBox::Yes) {
            if(codeEditor->saveFile()) {
                saveSuccessAction(codeEditor);
            }
            return;
        } else if (btn == QMessageBox::Cancel) {
            // 对话框的关闭按钮是与QMessageBox::question里面最后一个值绑定的
            return;
        }
    }
    ui->tabWidget->removeTab(index);
    delete codeEditor;
    initAction();
}

void MainWindow::saveSuccessAction(MyCodeEditor * codeEditor)
{
    QString fileName = codeEditor->getFileName();
    // 保存最近打开文件的历史记录
    saveHistory(fileName);
    // 设置tab标题
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), fileName);
    // 初始化菜单
    initMenu();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(ui->tabWidget->count() > 0) {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "警告", "有保存的文件，确定要关闭吗？",
                              QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
        }
    }
}

