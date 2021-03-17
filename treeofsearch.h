#ifndef TREEOFSEARCH_H
#define TREEOFSEARCH_H
#include "basetree.h"
#include <algorithm>

template<class X> class TreeOfSearch : public BaseTree<X>
{
public:

    TreeOfSearch();

    TreeOfSearch(TreeOfSearch<X> &);

    TreeOfSearch(X &);

    TreeOfSearch(X *, int);

    TreeOfSearch<X> &operator = (TreeOfSearch<X> &);

    virtual bool AddNode(X &);

    virtual bool DelNode(X &);

    // функции не виртуальные, т.к. для всех деревьев поиска одинаковые

    bool FindNode(X &);

    X FindMax();

    X FindMin();

private:

    typename TreeOfSearch<X>::BaseNode **SearchNode(X &, typename TreeOfSearch<X>::BaseNode *&);
};

template<class X> TreeOfSearch<X>::TreeOfSearch(): TreeOfSearch<X>::BaseTree()
{

}

template<class X> TreeOfSearch<X>::TreeOfSearch(TreeOfSearch &obj): TreeOfSearch<X>::BaseTree(obj)
{

}

template<class X> TreeOfSearch<X>::TreeOfSearch(X &data) : TreeOfSearch<X>::BaseTree(data)
{

}

template<class X> TreeOfSearch<X>::TreeOfSearch(X *mas, int n)
{
    BaseTree<X>::root = nullptr;

    if (mas) {
        for (int i = 0; i < n; i++) {
            AddNode(mas[i]);
        }
    }
}

template<class X> TreeOfSearch<X> &TreeOfSearch<X>::operator = (TreeOfSearch<X> &obj)
{
    // смотрим, есть ли что добавлять
    if (obj.root) {
        // если есть что добавить, проверяем наличие готовых вершин
        if (BaseTree<X>::root)
            // если есть готовые, то передаем оператору присвоения для вершин
        {
            *BaseTree<X>::root = *(obj.root);
        } else
            // если готовых нет, создаем, работает конструктор копирования вершин
        {
            BaseTree<X>::root = new typename TreeOfSearch<X>::BaseNode(*(obj.root));
        }
    } else

        // если копировать нечего, надо удалить наши вершины
        if (BaseTree<X>::root) {
            delete BaseTree<X>::root;
            BaseTree<X>::root = nullptr;
        }

    return *this;
}

template<class X> bool TreeOfSearch<X>::AddNode(X &data)
{
    // указатель куда будем вставлять элемент
    typename TreeOfSearch<X>::BaseNode **addto = &(BaseTree<X>::root);

    // ищем место для нового элемента
    while (*addto) {
        if ((*addto)->data > data) {
            addto = &((*addto)->lt);
        } else {
            addto = &((*addto)->rt);
        }
    }

    // т.к. использовали двойной указатель не надо проверять добавление в корень
    *addto = new typename TreeOfSearch<X>::BaseNode(data);

    if (*addto) {
        return true;
    }

    return false;
}

// доделана функция с одной из практик
template<class X> bool TreeOfSearch<X>::DelNode(X &data)
{
    typename TreeOfSearch<X>::BaseNode *del_node, **par_node, *rep_node;
    del_node = rep_node = nullptr;
    par_node = SearchNode(data, del_node);

    // проверяем нашли ли мы удаляемый элемент
    if (del_node) {
        // проверка del_node "проходная" вершина
        if (!del_node->lt) {
            rep_node = del_node->rt;
        } else if (!del_node->rt) {
            rep_node = del_node->lt;
        } else {
            // если у del_node есть оба поддерева, ищем замещающий элемент в левом поддереве
            typename TreeOfSearch<X>::BaseNode *rep_par_node = del_node;
            rep_node = del_node->lt;

            // спускаемся к самому правому элементу в поддереве
            while (rep_node->rt) {
                rep_par_node = rep_node;
                rep_node = rep_node->rt;
            }

            // проверяем где мы оказались
            // если заменяемый элемент не сын del_node, следовательно он будет находиться на правом указателе
            // то надо этот указатель переобозначить левой частью замещаемого элемента
            // а самому замещаемому элементу присвоить новую левую часть
            if (del_node != rep_par_node) {
                rep_par_node->rt = rep_node->lt;
                rep_node->lt = del_node->lt;
            }

            // правую часть заменяемого элемента всегда переобозначаем одинаково, т.к. ее никогда нет по принципу выбора
            rep_node->rt = del_node->rt;
        }


        *par_node = rep_node;
        del_node->lt = del_node->rt = nullptr;
        delete del_node;
        return true;
    }

    return false;
}

// идем по сравнениям
template<class X> bool TreeOfSearch<X>::FindNode(X &data)
{
    typename TreeOfSearch<X>::BaseNode *ptr = BaseTree<X>::root;

    while (ptr && ptr->data != data) {
        if (ptr->data > data) {
            ptr = ptr->lt;
        } else {
            ptr = ptr->rt;
        }
    }

    if (!ptr) {
        return false;
    }

    return true;
}

// идем всегда вправо
template<class X> X TreeOfSearch<X>::FindMax()
{
    X max;
    typename TreeOfSearch<X>::BaseNode *ptr = nullptr;

    if (BaseTree<X>::root) {
        ptr = BaseTree<X>::root;
        max = ptr->data;

        while (ptr->rt) {
            ptr = ptr->rt;
            max = ptr->data;
        }
    }

    return max;
}

// идем всегда влево
template<class X> X TreeOfSearch<X>::FindMin()
{
    X min;
    typename TreeOfSearch<X>::BaseNode *ptr = nullptr;

    if (BaseTree<X>::root) {
        ptr = BaseTree<X>::root;
        min = ptr->data;

        while (ptr->lt) {
            ptr = ptr->lt;
            min = ptr->data;
        }
    }

    return min;
}

// если нашли, то возвращаем указатель, если не нашли, возвращаем nullptr
template<class X> typename TreeOfSearch<X>::BaseNode **TreeOfSearch<X>::SearchNode(X &data,
        typename TreeOfSearch<X>::BaseNode *&found_node)
{
    // начинаем поиски с root-а
    typename TreeOfSearch<X>::BaseNode **parent_node = &(BaseTree<X>::root);
    found_node = BaseTree<X>::root;

    // ищем, пока дерево не закончится, либо пока не найдем нужный элемент
    while (found_node && found_node->data != data) {
        if (found_node->data > data) {
            parent_node = &(found_node->lt);
            found_node = found_node->lt;
        } else {
            parent_node = &(found_node->rt);
            found_node = found_node->rt;
        }
    }

    if (!found_node) {
        parent_node = nullptr;
    }

    return parent_node;
}

#endif // TREEOFSEARCH_H
