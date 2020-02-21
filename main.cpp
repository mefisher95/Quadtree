#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"
#include "GameObject.h"
#include "QTNode.h"


void draw_boundaries(QTNode* n)
{
    n->draw_boundaries();
    if (n->is_leaf()) return;
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            draw_boundaries(n->children(i));
        }
    }
}

void draw_game_objects(std::vector< GameObject* > &circles)
{
    for (int i = 0; i < NUM_CIRCLES; ++i)
    {
        circles[i]->draw();
    }
}

bool are_colliding(std::vector< GameObject* > &circles, int i, int j)
{
    int x_diff = abs(circles[i]->x() - circles[j]->x());
    int y_diff = abs(circles[i]->y() - circles[j]->y());
    int distance = sqrt(pow(x_diff, 2) + pow(y_diff, 2));
    int radius = circles[i]->radius();

    if(i != j && distance <= radius * 2) return true;
    else return false;
}

void update_GameObject(GameObject* &x, GameObject* &y)
{
    y->toggle_color();
    x->toggle_color();                    
    y->toggle_direction();
    x->toggle_direction();
    y->move();
    x->move();  
}

void calculate_parent_collisions(QTNode* p)
{
    QTNode* q = p->parent();
    for(int i = 0; i < q->num_circles(); ++i)
    {
        for(int j = 0; j < p->num_circles(); ++j)
        {
            int x_diff = abs(q->circles(i)->x() -
                             p->circles(j)->x());
            int y_diff = abs(q->circles(i)->y() -
                             p->circles(j)->y());
            int distance = sqrt(pow(x_diff, 2) + pow(y_diff, 2));
            int radius = q->circles(i)->radius();
                
            if(distance <= radius * 2)
            {
                update_GameObject(q->circles(i), p->circles(j));   
            }
        }
    }
}

void calculate_quadtree_collisions(QTNode* n)
{
    if(n->is_leaf())
    {
        for(int i = 0; i < n->num_circles(); ++i)
        {
            for(int j = 0; j < n->num_circles(); ++j)
            {   
                int x_diff = abs(n->circles(i)->x() -
                                 n->circles(j)->x());
                int y_diff = abs(n->circles(i)->y() -
                                 n->circles(j)->y());
                int distance = sqrt(pow(x_diff, 2) + pow(y_diff, 2));
                int radius = n->circles(i)->radius();
                
                if(i != j && distance <= radius * 2)
                {
                    update_GameObject(n->circles(i), n->circles(j));
                }
            }
            if(n->parent_contains_objects())
            {
                calculate_parent_collisions(n);
            }
        }
        return;
    }
    else
    {
        for(int i = 0; i < 4; ++i)
        {
            calculate_quadtree_collisions(n->children(i));
        }
    }
}


void get_collisions(std::vector< GameObject * > & circles, bool use_quadtrees,
                     QTNode* root)
{
    if(use_quadtrees)
    {
        calculate_quadtree_collisions(root);   
    }
    else
    {
        for(int i = 0; i < NUM_CIRCLES; ++i)
        {
            for(int j = 0; j < NUM_CIRCLES; ++j)
            {
                if(are_colliding(circles, i, j))
                {
                    update_GameObject(circles[i], circles[j]);
                }
            }
        }
    }
}

/****************************************************************************/
int main(int argc, char* argv[])
{
    Surface surface(W, H);
    Event event;
    bool use_quadtrees = true;

    std::vector< GameObject * > circles;
    for(int i = 0; i < NUM_CIRCLES; ++i)
    {
        GameObject * p = new GameObject(surface);
        circles.push_back(p);
    }

    while(1)
    {
        // hold space to run without using quadtrees- let go to use quadtrees
        if (event.poll() && event.type() == QUIT) break;
        else if (event.type() == KEYDOWN)
        {
            use_quadtrees = false;
        }
        else if (event.type() == KEYUP)
        {
            use_quadtrees = true;
        }
        
        //move circles
        for(int i = 0; i < circles.size(); ++i)
        {
            circles[i]->move();
        }

        // create new quad tree at each iteration
        QTNode* root = new QTNode(0, 0, W - 1, H - 1, circles,
                                  surface, NULL);
        root->create_quadtree(); 

        // collision detection
        get_collisions(circles, use_quadtrees, root);
        
        //Moving and drawing
        surface.lock();
        surface.fill(WHITE);
        draw_game_objects(circles);
        if(use_quadtrees) draw_boundaries(root);
        surface.unlock();
        surface.flip();
        delay(10);
    }

    return 0;
}


