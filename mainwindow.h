#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QFrame>
#include <QLineEdit>
#include <QCheckBox>
#include <QRect>
#include <QVBoxLayout>
#include <QGroupBox>
#include "drawing_area.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    // т.к. в классе "canvas" слоты принимают на вход int, следовательно необходимы соответствующие сигналы
    void create_tree(int count);

    void add_value(int value);

    void delete_value(int value);

    void find_value(int value);

private slots:
    void emit_create_tree_signal();

    void emit_add_value_signal();

    void emit_delete_value_signal();

    void emit_find_value_signal();


private:
drawing_area *canvas;


// элементы интерфейса
QPushButton *create_tree_butt;
QPushButton *add_value_butt;
QPushButton *del_value_butt;
QPushButton *find_value_butt;
QPushButton *clear_butt;
QRadioButton *rand_butt;
QRadioButton *search_butt;
QRadioButton *balanced_butt;
QLineEdit *line1;
QLineEdit *line2;
QFrame *frame;
QLabel *label1;
QLabel *label2;
QVBoxLayout *main_box;
QGroupBox *options_box;
QGroupBox *processing_box;

};
#endif // MAINWINDOW_H
