#ifndef RANDTREE_H
#define RANDTREE_H

#include <random>
#include "basetree.h"


template<class X> class RandTree : public BaseTree<X>
{
public:
    RandTree(){}

    RandTree(RandTree<X> &obj):BaseTree<X>(obj){}

    virtual bool AddNode(X &);

    virtual bool DelNode(X &);

    virtual bool FindNode(X &);

    virtual X FindMax(){}

    virtual X FindMin(){}

    virtual ~RandTree(){}

private:

};

template<class X> bool RandTree<X>::AddNode(X &value)
{
    typename BaseTree<X>::BaseNode** ptr = &(BaseTree<X>::root);

    while(*ptr)
    {
        if(rand()%2)
            ptr = &((*ptr)->lt);
        else
            ptr = &((*ptr)->rt);
    }
    (*ptr) = new typename RandTree<X>::BaseNode(value);
    if(*ptr)
        return true;
    return false;
}

template<class X> bool RandTree<X>::DelNode(X &value)
{
    if(RandTree<X>::root)
    {
        typename BaseTree<X>::BaseNode** iter = &(BaseTree<X>::root);
        std::stack<typename BaseTree<X>::BaseNode **> buf_nodes;
        buf_nodes.push(nullptr);

        while(iter && (*iter)->data != value)
        {
            if ((*iter)->rt) {
                buf_nodes.push(&((*iter)->rt));
            }

            if ((*iter)->lt) {
                iter = &((*iter)->lt);
            } else {
                iter = buf_nodes.top();
                buf_nodes.pop();
            }
        }

        if(iter)
        {
            typename RandTree<X>::BaseNode *ptr_1 = nullptr, *ptr_2 = nullptr;

            if((*iter)->lt)
                ptr_1 = (*iter)->lt;
            if((*iter)->rt)
            {
                if(ptr_1)
                    ptr_2 = (*iter)->rt;
                else
                    ptr_1 = (*iter)->rt;
            }

            delete (*iter);
            (*iter) = ptr_1;

            if(ptr_2)
            {
                typename BaseTree<X>::BaseNode** ptr= &(BaseTree<X>::root);
                while(*ptr)
                {
                    if(rand()%2)
                        ptr = &((*ptr)->lt);
                    else
                        ptr = &((*ptr)->rt);
                }
                (*ptr) = ptr_2;
            }

            return true;
        }
    }
    return false;
}

template<class X> bool RandTree<X>::FindNode(X &value)
{
    typename RandTree<X>::BaseNode* iter = RandTree<X>::root;
    std::stack<typename BaseTree<X>::BaseNode*> buf_nodes;
    buf_nodes.push(nullptr);

    while(iter && iter->data != value)
    {
        if (iter->rt) {
            buf_nodes.push(iter->rt);
        }

        if (iter->lt) {
            iter = (iter->lt);
        } else {
            iter = buf_nodes.top();
            buf_nodes.pop();
        }
    }

    if(iter)
        return true;
    return false;
}




#endif // RANDTREE_H
