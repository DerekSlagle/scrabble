#ifndef LVL1_H_INCLUDED
#define LVL1_H_INCLUDED

#include "Level.h"

class lvl1 : public Level
{
    public:
    

    // functions
    lvl1():Level() { std::cout << "Hello from lvl1 ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~lvl1() { cleanup(); }
};

#endif // LVL1_H_INCLUDED
