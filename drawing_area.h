#ifndef DRAWING_AREA_H
#define DRAWING_AREA_H

#include <QMainWindow>
#include "basetree.h"
#include "RandTree.h"
#include "treeofsearch.h"
#include "balancedtreeofsearch.h"

class drawing_area : public QWidget
{
    Q_OBJECT
public:
    explicit drawing_area(QWidget *parent = nullptr);
    ~drawing_area();

public slots:

    void create_tree(int count);

    void add_value(int value);

    void delete_value(int value);

    void find_value(int value);

    void clear_tree();

    void set_tree();

private:
    void draw_tree(BaseTree<int>::BaseNode*, int h, QRect draw_rect, QPainter &painter);

    void paintEvent(QPaintEvent *event);

    BaseTree<int>** active_tree;

    RandTree<int>* rand_tree;
    TreeOfSearch<int>* search_tree;
    BalancedTreeOfSearch<int>* balanced_tree;

    // размер всего поля для рисования
    QRect area_rect;
    // место, значение и флаг для "найденой" вершины, чтобы отмечать ее на рисунке
    // тут можно было бы сделать список и отмечать все найденные вершины
    QRect mark_node_rect;
    int mark_node_value;
    bool some_marked_node;
};

#endif // DRAWING_AREA_H
