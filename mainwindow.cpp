#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QSettings>

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#endif
#endif

QList<QString> getHistory();
void saveHistory(QString path);

QSettings *mSettings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置文本框居中
    this->setCentralWidget(ui->textEdit);
    if (mSettings == NULL) {
        mSettings = new QSettings("settings.ini", QSettings::IniFormat);
    }

    initMenu();

#if !QT_CONFIG(printer)
    ui->print->setEnabled(false);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMenu()
{
    QMenu *recent = this->findChild<QMenu *>("recent");
    QSet<QObject *> chList = recent->children().toSet();
    foreach (QObject *ch, chList) {
        QAction *action = (QAction *) ch;
        recent->removeAction(action);
    }

    QList<QString> list = getHistory();

    for(int i = list.size() - 1; i >= 0; i--) {
        recent->addAction(list[i], this, &MainWindow::on_open_recent_file);
    }

    if(list.size() > 0) {
        recent->addAction("清除历史记录", this, &MainWindow::on_clear_history_triggered, QKeySequence("Ctrl+Alt+Shift+C"));
    }
}



QList<QString> getHistory() {
    int size = mSettings->beginReadArray("history");

    QList<QString> list;
    for(int i = 0; i < size; i++) {
        mSettings->setArrayIndex(i);
        QString path = mSettings->value("path").toString();
        list.append(path);
        qDebug() << i << ":" << path;
    }

    mSettings->endArray();
    return list;
}

void saveHistory(QString path) {

    QList<QString> list = getHistory();

    // 不能使用list = list.toSet().toList()，会导致顺序乱了
    foreach(QString str, list) {
        if(str == path) {
            list.removeOne(str);
        }
    }
    list.append(path);

    mSettings->endArray();

    mSettings->beginWriteArray("history");

    for(int i = 0; i < list.size(); i++) {
        mSettings->setArrayIndex(i);
        // save string
        mSettings->setValue("path", list[i]);
    }

    mSettings->endArray();
}

// 新建文件
void MainWindow::on_new_file_triggered()
{
    qDebug() << "Start Create New File...";
    currentFile.clear();
    ui->textEdit->setText("");
}


void MainWindow::on_open_recent_file()
{
    QAction *action = (QAction *) sender();
    QString fileName = action->text();
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "无法打开此文件，报错信息：\n" + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();

    saveHistory(currentFile);
}

// 打开文件
void MainWindow::on_open_file_triggered()
{
    // 获得打开文件弹出框的文件名称
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件");
    // 得到文件句柄
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text) ) {
        QMessageBox::warning(this, "警告", "无法打开此文件，错误信息：\n" + file.errorString());
        return;
    }
    currentFile = fileName;
    // 设置标题为文件名
    setWindowTitle(fileName);
    // 读取文件内容
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();

    saveHistory(currentFile);
    initMenu();
}

// 保存文件
void MainWindow::on_save_file_triggered()
{
    QString fileName;
    if (currentFile.isEmpty()) {
        // 提示用户输入文件名，并获取保存文件的文件名
        fileName = QFileDialog::getSaveFileName(this, "保存文件");
        currentFile = fileName;
    } else {
        fileName = currentFile;
    }
    QFile file(fileName);
    // 处理文件保存异常
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "无法保存文件，报错信息：\n" + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    // 保存文件内容
    QTextStream out(&file);
    QString text = ui->textEdit->toHtml();
    out << text;
    file.close();

    saveHistory(currentFile);
    initMenu();
}

// 另存为
void MainWindow::on_save_as_triggered()
{
    // 获取另存为的文件名
    QString fileName = QFileDialog::getSaveFileName(this, "另存文件");

    QFile file(fileName);
    // 处理另存为文件异常
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "无法保存文件，报错信息：\n" + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    // 保存文件内容
    QTextStream out(&file);
    QString text = ui->textEdit->toHtml();
    out << text;
    file.close();

    saveHistory(currentFile);
    initMenu();
}

// 粘贴
void MainWindow::on_paste_triggered()
{
    ui->textEdit->paste();
}

// 剪切
void MainWindow::on_cut_triggered()
{
    ui->textEdit->cut();
}

// 复制
void MainWindow::on_copy_triggered()
{
    ui->textEdit->copy();
}

// 字体
void MainWindow::on_font_triggered()
{
    bool fontSelected;
    // 打开字体对话框
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected) {
        // 设置字体
        ui->textEdit->setFont(font);
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
    ui->textEdit->undo();
}

// 取消撤销
void MainWindow::on_redo_triggered()
{
    ui->textEdit->redo();
}

// 退出
void MainWindow::on_exit_triggered()
{
    QCoreApplication::exit();
}

// 打印
void MainWindow::on_print_triggered()
{
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
    ui->textEdit->print(&printDev);
#endif
}

// 加粗
void MainWindow::on_bolder_triggered(bool bolder)
{
    ui->textEdit->setFontWeight(bolder?QFont::Bold:QFont::Normal);
}

// 斜体
void MainWindow::on_italic_triggered(bool italic)
{
    ui->textEdit->setFontItalic(italic);
}

// 下划线
void MainWindow::on_underline_triggered(bool underline)
{
    ui->textEdit->setFontUnderline(underline);
}


void MainWindow::on_clear_history_triggered()
{
    mSettings->remove("history");
    initMenu();
}
