#pragma once

class i_game_event_listener2
{

public:

    virtual ~i_game_event_listener2() {}
    virtual void fire_game_event(i_game_event* event) = 0;

};