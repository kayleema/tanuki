#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QScrollBar"
#include "Environment.h"
#include "Context.h"

#include <iostream>

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
    ui->lineEdit->setFocus();

    env = new Environment(new Context());
}

MainWindow::~MainWindow() {
    delete ui;
    delete env;
}

void MainWindow::on_lineEdit_returnPressed() {
    std::wstring line = ui->lineEdit->text().toStdWString();
    bool started = false;
    if (!text.empty()) {
        started = true;
        text = text + L"\n";
    }
    text = text + line;
    ui->lineEdit->clear();


    auto stringInput = StringInputSource(text.c_str());
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    tokens = testTokenizer.getAllTokens();

    ui->textEdit->append( QString::fromStdWString(text));
    if (!isComplete(tokens)) {
    } else {
        auto bufferedTokenizer = BufferedTokenizer(tokens);
        auto parser = Parser(&bufferedTokenizer);
        SyntaxNode *tree = parser.run();

        Value *result = env->eval(tree->children[0]);

        ui->textEdit->append(
                QString("結果：") + QString::fromStdString(result->toString())
        );

        tokens.clear();
        text = L"";
    }
    ui->textEdit->verticalScrollBar()->setValue(
            ui->textEdit->verticalScrollBar()->maximum()
    );
}
