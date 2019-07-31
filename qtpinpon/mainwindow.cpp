#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QScrollBar"
#include "src/Environment.h"
#include "src/Context.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->textEdit->clear();
    for (int var = 0; var < height() / 16; ++var) {
        ui->textEdit->append("\n");
    }
    ui->textEdit->verticalScrollBar()->setValue(
            ui->textEdit->verticalScrollBar()->maximum());

    env = new Environment(new Context());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed() {
    std::wstring line = ui->lineEdit->text().toStdWString();
    ui->lineEdit->clear();


    auto stringInput = StringInputSource(line.c_str());
    auto testTokenizer = FileTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();

    Value *result = env->eval(tree->children[0]);


    QString newLine =
            QString("＞：") + QString::fromStdWString(line).toHtmlEscaped() +
            QString("<br/>") +
            QString::fromStdString(result->toString()).toHtmlEscaped() +
            QString("<br/>");
    ui->textEdit->append(newLine);
    ui->textEdit->verticalScrollBar()->setValue(
            ui->textEdit->verticalScrollBar()->maximum());
}
