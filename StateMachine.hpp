#pragma once

class StateMachine
{
public:
    void requestState( int state ) 
    {
        requestedState = state;
    }
    void setState( int state ) 
    {
        this->state = state;
    }
    void setRequestedState()
    {
        if (requestedState != state)
        {
            state = requestedState;
        }
    }
private:
    int state;
    int requestedState;
};
