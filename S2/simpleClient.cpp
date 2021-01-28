#include <iostream>

#include "olc_net.h"

enum class CustomMsgTypes : uint8_t
{
    FireBulet,
    MovePlayer
};

int main()
{
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::FireBulet;

    int a = 1;
    bool b = true;
    float e = 2.71f;

    struct 
    {
        float x;
        double y;
    } c[3];

    c[0].x = 0.1;
    c[0].y = 0.2;

    c[1].x = 1;
    c[1].y = 2;

    c[2].x = 10;
    c[2].y = 20;

    msg << a << b << c << e;

    a = 5;
    b = false;
    e = 7.0f;
    c[0] = {0,0};
    c[1] = {0,0};
    c[2] = {0,0};

    msg >> e >> c >> b >> a;

    return 0;
}