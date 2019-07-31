#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/Context.h"
#include "src/Environment.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private
    slots:
            void

    on_lineEdit_returnPressed();

private:
    Ui::MainWindow *ui;

    Environment *env;
};

#endif // MAINWINDOW_H
