#include "drawing_area.h"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

#include "basetree.h"
#include "RandTree.h"
#include "treeofsearch.h"
#include "balancedtreeofsearch.h"

drawing_area::drawing_area(QWidget *parent) : QWidget(parent)
{
    // задаем размер поля для рисования
    QRect main_rect = parent->rect();
    area_rect = QRect(5, 5, (float)main_rect.width()*0.8 - 10, main_rect.height() - 10);
    this->setGeometry(area_rect);

    // задаем фон для поля
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(QPalette::Background,Qt::white));

    // создаем деревья
    rand_tree = new RandTree<int>;
    search_tree = new TreeOfSearch<int>;
    balanced_tree = new BalancedTreeOfSearch<int>;
    active_tree = reinterpret_cast<BaseTree<int>**>(&search_tree);

    some_marked_node = false;
}

// функция генерации дерева
void drawing_area::create_tree(int count)
{
    some_marked_node = false;
    QFile tree("tree.txt");
    tree.open(QFile::WriteOnly);

    QTextStream out(&tree);
    QRandomGenerator generator;
    (*active_tree)->ClearTree();
    // герерируем значение, записываем в файл и добавляем в дерево
    for(int i = 0; i < count; i++)
    {
        int gen = generator.bounded(100);
        out << gen << ' ';
        (*active_tree)->AddNode(gen);
    }

    // сохраняем файл
    tree.close();
    // обновляем картинку
    emit update();
}
// добавление заданного значения в дерево
void drawing_area::add_value(int value)
{
    some_marked_node = false;
    QFile tree("tree.txt");
    // проверка открытия файла
    // открываем файл на запись в конец
    if(tree.open(QIODevice::Append))
    {
        QTextStream out(&tree);
        // записываем в файл и добавляем в дерево
        out << value << ' ';
        (*active_tree)->AddNode(value);

        tree.close();
        // обновляем картинку
        emit update();
    }
}
// функция удаления заданного значения из дерева
void drawing_area::delete_value(int value)
{
    some_marked_node = false;
    // проверяем наличие заданной вершины в дереве
    if((*active_tree)->FindNode(value))
    {
        // файл изначального дерева
        QFile tree("tree.txt");
        // вспомогательный файл для перезаписи без удаляемой вершины
        QFile buf("buf.txt");

        if(tree.open(QIODevice::ReadOnly) && buf.open(QIODevice::WriteOnly))
        {
            QTextStream treeF(&tree);
            QTextStream bufF(&buf);
            int x, y;
            y = value;
            bool deleted = false;
            // переписываем все, кроме первого попавшегося значения равного value
            while(!treeF.atEnd())
            {
                treeF >> x;
                if((x != y || deleted) && !treeF.atEnd())
                {
                    bufF << x << ' ';
                }
                if(x == y)
                    deleted = true;
            }
            tree.close();
            buf.close();
            tree.open(QIODevice::WriteOnly);
            // пришлось создать еще 1 поток, т.к. была проблема
            // после открытия на запись, при помощи потока treeF не выводились данные в файл, хотя поток bufF считывает правильно
            // и следовательно оставался пустой файл
            QTextStream treeout(&tree);
            buf.open(QIODevice::ReadOnly);
            // переписываем обратно
            while(!bufF.atEnd())
            {
                bufF >> x;
                if(!bufF.atEnd())
                    treeout << x << ' ';
            }

            (*active_tree)->DelNode(value);

            tree.close();
            buf.close();
            buf.remove();
            emit update();
        }
    }
}
// ищем вершину с заданным значением
void drawing_area::find_value(int value)
{
    some_marked_node = false;
    // проверяем наличие дерева
    if((*active_tree)->root)
    {
        // создаем итератор и стек, как в обходе в классе basetree, но из пар значение+координаты
        std::pair<BaseTree<int>::BaseNode*,QRect> iter;
        std::stack<std::pair<BaseTree<int>::BaseNode*,QRect>> buf_nodes;

        // для посика вершины используется обход NLR, т.к. его можно использовать как для случайного дерева, так и для дерева поиска
        iter.first = (*active_tree)->root;
        iter.second = QRect(50*((1<<(BaseTree<int>::height(iter.first)-1))-1),10,50,50);

        buf_nodes.push(std::pair<BaseTree<int>::BaseNode*,QRect>(nullptr,QRect(0,0,0,0)));

        // ходим, пока не найдем вершину, или пока не просмотрим все вершины дерева
        while(iter.first && iter.first->data != value)
        {
            // вычисляем координаты левых и правых детей как в функции рисования
            if (iter.first->rt) {
                buf_nodes.push(std::pair<BaseTree<int>::BaseNode*,QRect>(iter.first->rt,QRect(iter.second.x()+50*(1<<(BaseTree<int>::height(iter.first)-2)),iter.second.y()+80,50,50)));
            }

            if (iter.first->lt) {
                iter = std::pair<BaseTree<int>::BaseNode*,QRect>(iter.first->lt,QRect(iter.second.x()-50*(1<<(BaseTree<int>::height(iter.first)-2)),iter.second.y()+80,50,50));
            } else {
                iter = buf_nodes.top();
                buf_nodes.pop();
            }
        }

        // записываем координаты найденой вершины
        // говорим, нашли ли мы вершину или нет
        if(iter.first)
        {
            mark_node_value = value;
            mark_node_rect = iter.second;
            some_marked_node = true;
        }
    }
    emit update();
}
// функция очистки дерева
void drawing_area::clear_tree()
{
    some_marked_node = false;
    search_tree->ClearTree();
    balanced_tree->ClearTree();

    QFile tree("tree.txt");
    tree.open(QFile::WriteOnly);
    tree.close();
    emit update();
}
// функция, которая рекурсивно рисует дерево, учитывая высоту, чтобы не было наложений вершин
// родитель находится ровно посеридине между своими детьми
// в функции не учитывается заполненность дерева, считается, что на всех уровнях присутствуют все вершины
// из-за этого с увеличением высоты дерева быстро растет занимаемое простравнство в ширину
void drawing_area::draw_tree(BaseTree<int>::BaseNode* draw_node, int h, QRect draw_rect, QPainter &painter)
{
    // рисуем вершину, ставим значение
    painter.drawEllipse(draw_rect);
    QString str;
    str.setNum(draw_node->data);
    painter.drawText(QPoint(draw_rect.x()+15,draw_rect.y()+30), str);

    // рисуем связь и вызываем функцию для левых и правых поддеревьев
    if(draw_node->lt)
    {
        painter.drawLine(draw_rect.x()+25,draw_rect.y()+50,draw_rect.x() - 50*(1<<(h-2))+25,draw_rect.y()+80);
        draw_tree(draw_node->lt,h-1,QRect(draw_rect.x()-50*(1<<(h-2)),draw_rect.y()+80,50,50),painter);
    }
    if(draw_node->rt)
    {
        painter.drawLine(draw_rect.x()+25,draw_rect.y()+50,draw_rect.x() + 50*(1<<(h-2)) + 25,draw_rect.y()+80);
        draw_tree(draw_node->rt,h-1,QRect(draw_rect.x()+50*(1<<(h-2)),draw_rect.y()+80,50,50),painter);
    }
}
// вызываем функцию рисования дерева и отмечаем "наденную" вершину, если таковая есть
void drawing_area::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::black));
    if((*active_tree)->root)
    {
        int h = (*active_tree)->Height();
        draw_tree((*active_tree)->root, h, QRect(50*((1<<(h-1))-1),10,50,50),painter);
    }

    if(some_marked_node)
    {
        painter.setPen(QPen(Qt::red));
        painter.drawEllipse(mark_node_rect);
        QString str;
        str.setNum(mark_node_value);
        painter.drawText(QPoint(mark_node_rect.x()+15,mark_node_rect.y()+30), str);
    }
}
// функция выбора дерева
void drawing_area::set_tree()
{
    // определяем какая кнопка дала сигнал
    // выбираем дерево
    QString tree_name = QObject::sender()->objectName();
    if(tree_name == "rand_tree")
        active_tree = reinterpret_cast<BaseTree<int>**>(&rand_tree);
    if(tree_name == "search_tree")
        active_tree = reinterpret_cast<BaseTree<int>**>(&search_tree);
    if(tree_name == "balanced_tree")
        active_tree = reinterpret_cast<BaseTree<int>**>(&balanced_tree);

    // читим дерево и заполняем заново, используя файл
    (*active_tree)->ClearTree();
    QFile tree("tree.txt");
    tree.open(QFile::ReadOnly);
    QTextStream in(&tree);

    if(!in.atEnd())
    {
        int val;
        in >> val;
        while(!in.atEnd())
        {
            (*active_tree)->AddNode(val);
            in >> val;
        }
    }
    tree.close();
    if(some_marked_node)
        find_value(mark_node_value);
    emit update();
}

// удаляем деревья, т.к. они не удаляются автоматически
// чистим файл с вершинами
drawing_area::~drawing_area()
{
    QFile tree("tree.txt");
    tree.open(QFile::WriteOnly);
    tree.close();
    if(search_tree){
        delete search_tree;
        search_tree = nullptr;
    }
    if(balanced_tree){
        delete balanced_tree;
        balanced_tree = nullptr;
    }
    if(rand_tree){
        delete rand_tree;
        rand_tree = nullptr;
    }
    active_tree = nullptr;
}
