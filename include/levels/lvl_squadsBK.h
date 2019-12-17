#ifndef LVL_SQUADS_H_INCLUDED
#define LVL_SQUADS_H_INCLUDED

#include "Level.h"
#include "../leg_types/legAdapter.h"
#include "../pathUtils.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"

class squad
{
    public:
    Leg* pLeg = nullptr;
    std::vector<impactTarget> ITvec;// adapter object movable + path motion
    std::vector<sf::CircleShape> circVec;// for inertAdaptVec
    std::vector<sf::Text> healthMsgVec;// for inertAdaptVec
    sf::Texture ballTxtB;// for above balls
    sf::CircleShape circ;// template for vector of balls
    float startDelay = 0.0f, dtLaunch = 0.0f;
    float v0 = 0.0f;// launch speed
    float offX = 0.0f, offY = 0.0f;// offset to path
    bool anyAlive = true;

    bool init( std::istream& is, std::vector<Leg*>& PathVec );
    squad(){}
    squad( std::istream& is, std::vector<Leg*>& PathVec ) { init( is, PathVec ); }
    void update( float dt );
    void draw( sf::RenderTarget& RT ) const;
};

class lvl_squads : public Level
{
    public:
    sf::Text wasupMsg;
    std::vector<Leg*> pathVec;
    std::vector<squad> squadVec;
    inertialAdapter flingIA;
    sf::CircleShape flingBall;
    float flingMass = 3.0f;
    sf::Texture ballTxtA;// for above

    // some ui
    buttonValOnHit gradStrip, dVelStrip;
    // motion control on paths
    controlSurface pathControl;
    buttonRect pathButt[5];// vel+, vel-, brake, <-, ->
    sf::CircleShape pathCirc;// color indicates which path is controlled
    void makePathControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    std::vector<squad>::iterator squadIter;
    float brakeRate = 0.5f, accRate = 0.2f;

    // functions
    lvl_squads():Level() { std::cout << "Hello from lvl_squads ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~lvl_squads() { cleanup(); }
};

#endif // LVL_SQUADS_H_INCLUDED
