#ifndef QTNODE_H
#define QTNODE_H

#include <iostream>
#include <vector>
#include "Surface.h"
#include "Constants.h"

class QTNode
{
public:
    QTNode(int x_min, int y_min, int x_max, int y_max,
           std::vector< GameObject* > & circles, Surface &surface,
           QTNode* parent = NULL)
        :x_min_(x_min), y_min_(y_min), x_max_(x_max), y_max_(y_max), parent_(parent),
        surface_(surface)
        {
            for (int i = 0; i < circles.size(); ++i)
            {
                circles_.push_back(circles[i]);
            }
        }

    // Getters/ Setters
    const int x_min() const { return x_min_; }
    const int y_min() const { return y_min_; }
    const int x_max() const { return x_max_; }
    const int y_max() const { return y_max_; }
    const int num_circles() const { return circles_.size(); }
    const int num_children() const { return children_.size(); }
    const QTNode * const parent() const { return parent_; }
    std::vector< GameObject * > circles() { return circles_; }
    QTNode *& parent() { return parent_; }


    // Booleans
    const bool is_root() const { return (parent_ == NULL); }
    const bool is_leaf() const
    {
        return (children_.size() > 0 ? false : true);
    }
    const bool has_children() const { return !this->is_leaf(); }
    const bool parent_contains_objects() const
    {
        if(!this->is_root() && parent_->contains_objects()) return true;
        return false;
    }
    const bool contains_objects() const
    {
        if(circles_[0] != NULL && circles_[0] != 0) return true;
        return false;
    }

    void create_quadtree()
    {
        _create(this);
    }
    
    void draw_boundaries()
    {
        surface_.put_line(x_min_, y_min_, x_max_, y_min_, 0, 0, 0);
        surface_.put_line(x_min_, y_max_, x_max_, y_max_, 0, 0, 0);
        surface_.put_line(x_min_, y_min_, x_min_, y_max_, 0, 0, 0);
        surface_.put_line(x_max_, y_min_, x_max_, y_max_, 0, 0, 0);
    }

    // Others
    QTNode * root()
    {
        QTNode * p = this;
        while(p->parent() != NULL)
        {
            p = p->parent();
        }
        return p;
    }

    int depth()
    {
        int count = 0;
        find_depth(count, this);
        return count;
    }
    
    void find_depth(int &count, const QTNode* const p)
    {
        if(p->parent() == NULL) return;
        else
        {
            ++count;
            return find_depth(count, p->parent());
        }
    }


    // return pointer to Game Objects
    const GameObject* circles(int index) const // rvalue cannot be changed
    {
        if(circles_.size() <= index) return NULL;
        else return circles_[index];
    }
    
    GameObject* &circles(int index) //lvalue can be changed
    {
        if(circles_.size() <= index) circles_.resize(index + 1);
        return circles_[index];
    }

        
    // return pointer to children
    const QTNode* children(int index) const // rvalue cannot be changed
    {
        if(children_.size() <= index) return NULL;
        else return children_[index];
    }
    
    QTNode* &children(int index) //lvalue can be changed
    {
        if(children_.size() <= index) children_.resize(index + 1);
        return children_[index];
    }

    
    // Circle-related
    void insert_circle(GameObject * c)
    {
        circles_.push_back(c);
    }
    void remove_circle(const int index)
    {
        circles_.erase(circles_.begin()+index);
    }
    void clear_circles()
    {
        circles_.clear();
    }
    void pop_back_circle()
    {
        circles_.pop_back();
    }

    
    // Child-related
    void insert_child(const int index, const int xmin, const int ymin,
                      const int xmax, const int ymax)
    {
        QTNode * p = new QTNode(xmin, ymin, xmax, ymax,
                                circles_, surface_, this);
        if(this->children(index) != 0 || this->children(index) != NULL
            || index < 0 || index > 3)
        {
            std::cout << "Invalid insertion of child" << std::endl;
        }
        else if(index > children_.size() - 1)
        {
            for(int i = children_.size() - 1; i < index; ++i)
            {
                this->children(i) = NULL;
            }
            this->children(index) = p;
        }
        else
        {
            this->children(index) = p;
        }
    }
    void remove_child(const int index) // remove the subtree at child i.
    {
        remove_subsequent(this->children(index));
        this->children(index) = NULL;
    }
    void remove_subsequent(const QTNode * p)
    {
        if(p->is_leaf())
        {
            delete p; return;
        }
        else
        {
            for(int i = 0; i < p->num_children(); ++i)
            {
                remove_subsequent(p->children(i));
            }
            delete p;
        }
    }
    
    
    // delete entire tree
    ~QTNode()
    {
        delete_quadtree(this);
    }
    
    void delete_quadtree(QTNode * p)
    {
        if(p->is_leaf())
        {
            delete p; return;
        }
        else
        {
            for(int i = 0; i < p->num_children(); ++i)
            {
                delete_quadtree(p->children(i));
            }
            delete p;
        }
    }

private:
    // private member variables
    int x_min_, y_min_, x_max_, y_max_;
    Surface & surface_;
    QTNode * parent_; // parent node
    std::vector< GameObject * > circles_; // keys
    std::vector< QTNode * > children_; // max 4 children
    
    void _create(QTNode* n)
    {
        if (n->num_circles() > THRESHOLD && n->depth() < MAX_DEPTH)
        {
            n->_create_children();
            n->_populate_quadtree(n);

            for (int i = 0; i < 4; ++i)
            {
                _create(n->children(i));
            }
        }
        else return;
    }

    void _create_children()
    {
        children_.push_back(new QTNode(x_min_, y_min_, (x_min_ + x_max_) / 2,
                                       (y_min_ + y_max_) / 2, circles_,
                                       surface_, this));
        children_.push_back(new QTNode((x_min_ + x_max_) / 2, y_min_, x_max_,
                                       (y_min_ + y_max_) / 2, circles_,
                                       surface_, this));
        
        children_.push_back(new QTNode(x_min_, (y_min_ + y_max_) / 2,
                                       (x_min_ + x_max_) / 2, y_max_, circles_,
                                       surface_, this));
        children_.push_back(new QTNode((x_min_ + x_max_) / 2, (y_min_ + y_max_) / 2,
                                       x_max_, y_max_, circles_, surface_, this));

        children_[0]->clear_circles();
        children_[1]->clear_circles();
        children_[2]->clear_circles();
        children_[3]->clear_circles();
    }

    void _populate_quadtree(QTNode* n)
    {
        std::vector< GameObject* > temp;
        for (int i = n->num_circles() - 1; i > -1; --i)
        {
            GameObject* x = n->circles(i);
            n->pop_back_circle();

            if (x->x() > n->children(0)->x_min() &&
                x->x() < n->children(0)->x_max() &&
                x->y() > n->children(0)->y_min() &&
                x->y() < n->children(0)->y_max())
            {
                n->children(0)->insert_circle(x);
            }
            else if (x->x() > n->children(1)->x_min() &&
                     x->x() < n->children(1)->x_max() &&
                     x->y() > n->children(1)->y_min() &&
                     x->y() < n->children(1)->y_max())
            {
                n->children(1)->insert_circle(x);
            }
            else if (x->x() > n->children(2)->x_min() &&
                     x->x() < n->children(2)->x_max() &&
                     x->y() > n->children(2)->y_min() &&
                     x->y() < n->children(2)->y_max())
            {
                n->children(2)->insert_circle(x);
            }
            else if (x->x() > n->children(3)->x_min() &&
                     x->x() < n->children(3)->x_max() &&
                     x->y() > n->children(3)->y_min() &&
                     x->y() < n->children(3)->y_max())
            {
                n->children(3)->insert_circle(x);
            }
            else temp.push_back(x);
        }

        for (int i = 0; i < temp.size(); ++i)
        {
            n->insert_circle(temp[i]);
        }
    }
};

#endif
