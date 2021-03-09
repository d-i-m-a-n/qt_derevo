#include "mainwindow.h"
#include <QLabel>
#include <QFrame>
#include <QLineEdit>
#include <QCheckBox>
#include <QRect>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // устанавливаем название окна и размер
    // я не разобрался как нормально применить компановщик, поэтому фиксированный размер у окна
    setWindowTitle("Trees");
    setGeometry(250,100,1500,800);
    setFixedSize(1500,800);

    // создаем область рисования
    canvas = new drawing_area(this);

    // дальше объявляем и объединяем все элементы интерфейса и даем им названия
    QRect main_rect = this->rect();
    QRect frame_rect;
    frame_rect.setX((float)main_rect.width()*0.8);
    frame_rect.setY(5);
    frame_rect.setHeight(main_rect.height() - 250);
    frame_rect.setWidth(main_rect.width() - frame_rect.x() - 5);

    frame = new QFrame(this);
    frame->setGeometry(frame_rect);
    frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    frame->setLineWidth(2);

    main_box = new QVBoxLayout(frame);
    main_box->addStretch(1);

    label1 = new QLabel("Count of nodes");
    line1 = new QLineEdit();
    // на поля ввода устанавливается маска, чтобы нельзя было вводить что-то кроме чисел
    line1->setInputMask("d0");
    create_tree_butt = new QPushButton("Create tree");
    options_box = new QGroupBox("Options");
    QVBoxLayout *layout = new QVBoxLayout();

    rand_butt = new QRadioButton("Random tree");
    search_butt = new QRadioButton("Tree of search");
    balanced_butt = new QRadioButton("Balancde tree of search");

    layout->addWidget(rand_butt);
    layout->addWidget(search_butt);
    layout->addWidget(balanced_butt);

    options_box->setLayout(layout);

    processing_box = new QGroupBox("Processing");
    layout = new QVBoxLayout();

    label2 = new QLabel("Value of node");

    line2 = new QLineEdit();
    // на поля ввода устанавливается маска, чтобы нельзя было вводить что-то кроме чисел
    line2->setInputMask("D000");

    add_value_butt = new QPushButton("Add value");
    del_value_butt = new QPushButton("Delete value");
    find_value_butt = new QPushButton("Find value");
    clear_butt = new QPushButton("Clear tree");

    layout->addWidget(label2);
    layout->addWidget(line2);
    layout->addWidget(add_value_butt);
    layout->addWidget(del_value_butt);
    layout->addWidget(find_value_butt);
    layout->addWidget(clear_butt);

    processing_box->setLayout(layout);

    main_box->addWidget(label1);
    main_box->addWidget(line1);
    main_box->addWidget(create_tree_butt);
    main_box->addWidget(options_box);
    main_box->addWidget(processing_box);

    // устанавливаю первоначальное состояние
    search_butt->setChecked(true);

    // соединяем сигналы и слоты для "mainwindow"
    // т.к. в классе "canvas" слоты принимают на вход int, следовательно необходимы соответствующие сигналы
    connect(create_tree_butt,SIGNAL(clicked()),this,SLOT(emit_create_tree_signal()));
    connect(add_value_butt,SIGNAL(clicked()),this,SLOT(emit_add_value_signal()));
    connect(del_value_butt,SIGNAL(clicked()),this,SLOT(emit_delete_value_signal()));
    connect(find_value_butt,SIGNAL(clicked()),this,SLOT(emit_find_value_signal()));

    // соединяем оставшиеся сигналы уже со слотами класса "canvas"
    connect(this,SIGNAL(create_tree(int)),canvas,SLOT(create_tree(int)));
    connect(this,SIGNAL(add_value(int)),canvas,SLOT(add_value(int)));
    connect(this,SIGNAL(find_value(int)),canvas,SLOT(find_value(int)));
    connect(this,SIGNAL(delete_value(int)),canvas,SLOT(delete_value(int)));

    connect(clear_butt,SIGNAL(clicked()),canvas,SLOT(clear_tree()));
    connect(rand_butt,SIGNAL(clicked()),canvas,SLOT(set_tree()));
    connect(search_butt,SIGNAL(clicked()),canvas,SLOT(set_tree()));
    connect(balanced_butt,SIGNAL(clicked()),canvas,SLOT(set_tree()));

    // даю название для кнопок, чтобы в классе "canvas" при переключении деревьев знать какая именно кнопка дала сигнал
    rand_butt->setObjectName("rand_tree");
    search_butt->setObjectName("search_tree");
    balanced_butt->setObjectName("balanced_tree");


//    QHBoxLayout *box = new QHBoxLayout();
//    box->addWidget(canvas);
//    box->addWidget(frame);
//    this->setLayout(box);
/*
    connect(this,SIGNAL(),canvas,SLOT());
    connect(this,SIGNAL(),canvas,SLOT());
    connect(this,SIGNAL(),canvas,SLOT());
    connect(this,SIGNAL(),canvas,SLOT());
    connect(this,SIGNAL(),canvas,SLOT());
    connect(this,SIGNAL(),canvas,SLOT());
*/
}

MainWindow::~MainWindow()
{

}
// получаем информацию из полей ввода, проверяя наличие хоть какой-то информации, и вызываем необходимый сигнал
// т.к. на полях стоят маски, то проверять дополнительно ничего не нужно
void MainWindow::emit_create_tree_signal()
{
    if(!line1->text().isEmpty())
    {
        int n = line1->text().toInt();
        emit create_tree(n);
    }
}

void MainWindow::emit_add_value_signal()
{
    if(!line2->text().isEmpty())
    {
        int value = line2->text().toInt();
        emit add_value(value);
    }
}

void MainWindow::emit_delete_value_signal()
{
    if(!line2->text().isEmpty())
    {
        int value = line2->text().toInt();
        emit delete_value(value);
    }
}

void MainWindow::emit_find_value_signal()
{
    if(!line2->text().isEmpty())
    {
        int value = line2->text().toInt();
        emit find_value(value);
    }
}
