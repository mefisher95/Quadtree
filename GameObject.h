#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject
{
public:
    GameObject(Surface &surface)
        :surface_(surface)
    {
        x_ = rand() % W;
        y_ = rand() % H;

        radius_ = 3;
        x_speed_ = 3.14;
        y_speed_ = 3.14;
        color1_ = true;

        int y_direct = rand() % 2;
        int x_direct = rand() % 2;

        if (y_direct) y_speed_ = -y_speed_;
        if (x_direct) x_speed_ = -x_speed_;

        r1_ = 255;
        g1_ = 000;
        b1_ = 000;

        r2_ = 000;
        g2_ = 000;
        b2_ = 255;
    }

    bool color() { return color1_; }
    void toggle_color() { color1_ = (color1_ ? false : true); }
    void toggle_direction() { x_speed_ = -x_speed_; y_speed_ = -y_speed_; }
    int x() { return x_; }
    int y() { return y_; }
    int radius() { return radius_; }

    void draw()
    {
        if (color1_) surface_.put_circle(x_, y_, radius_, r1_, g1_, b1_);
        else surface_.put_circle(x_, y_, radius_, r2_, g2_, b2_);
    }

    void move()
    {
        x_ += x_speed_;
        if (x_ < 0)
        {
            x_ = 0;
            x_speed_ = -x_speed_;
        }
        else if (x_ + radius_ * 2 > W - 1)
        {
            x_ = W - 1 - radius_ * 2;
            x_speed_ = -x_speed_;
        }

        y_ += y_speed_;
        if (y_ < 0)
        {
            y_ = 0;
            y_speed_ = -y_speed_;
        }
        else if (y_ + radius_ * 2 > H - 1)
        {
            y_ = H - 1 - radius_ * 2;
            y_speed_ = -y_speed_;
        }
    }

private:
    int x_;
    int y_;
    int radius_;
    double x_speed_;
    double y_speed_;
    int r1_, g1_, b1_;
    int r2_, g2_, b2_;
    bool color1_;
    Surface &surface_;
};

#endif
