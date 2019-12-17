#ifndef LVL_SQUADS_H_INCLUDED
#define LVL_SQUADS_H_INCLUDED

#include "Level.h"
#include "../leg_types/legAdapter.h"
#include "../leg_types/mouseLeg.h"
#include "../pathUtils.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/buttonList.h"
#include "../button_types/radioButton.h"
#include "../button_types/controlSurface.h"
#include "../Shot.h"
#include "../laser.h"

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
    float iHealth = 0.0f;// for each
    bool anyAlive = true;

    bool init( std::istream& is, std::vector<Leg*>& PathVec );
    void reStart();// same data as last
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
    mouseLeg flingLeg;
    sf::CircleShape flingBall;
    bool flingIsFree = true;// toggle with rt mouse button. See update()
    sf::Vector2f fireGrabPos;// to base drag direction for firing
    float fireTime = 0.0f, fireDelay = 8.0f;
    bool doFire = false;// spacebar toggles
    float flingMass = 3.0f;
    sf::Texture ballTxtA;// for above

    float loadTime = 0.0f;// time until next squad loads up
    unsigned int maxSquads = 5;// no load if all full
    std::ifstream squadIn;// open for duration. Load 1x1 until no more
    void loadSquad();

    // bullets?
 //   std::vector<freeMover> fmVec;
 //   std::vector<sf::CircleShape> fmCircVec;
    std::vector<Shot> shotVec;
    unsigned int maxShots = 20;
    float shotSpeed = 10.0f;
    float shotLife = 100.0f;
    buttonValOnHit shotMsStrip;
    multiSelector shotMS;
    controlSurface shotControl;// and the grad, dVel strips below
    void makeShotControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    Shot* getShot();
    unsigned int maxBombShots = 10;
    std::vector<ShotBomb> shotBombVec;
    ShotBomb* getShotBomb();
    bool doFireSB = false;// F3 toggles
    sf::Vertex xHair[4];// drawn where shotBomb goes boom if fired

    // clusterShot
    clusterShot clShot;// 1 instance at a time!
    buttonValOnHit clusterMsStrip;
 //   buttonValOnHit vFragStrip, tLifeStrip;
    multiSelector clusterMS;
    controlSurface clusterControl;
    buttonRect useSbButt;// use shotBombs as frags
    void makeClusterControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    bool doFireCS = false;// F4 assigns true. Single shot. False on fire in update()

    // guidedShot
    guidedShot gdShot;// 1 instance at a time!
    buttonValOnHit gdVelStrip, gdAtStrip, gdApStrip;
    controlSurface gdShotControl;
    void makeGuideShotControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    bool doFireGD = false;// F5 assigns true. Single shot. False on fire in update()

    // a bomb
    Bomb theBomb;
    buttonValOnHit bombStrip;
    multiSelector bombMS;
    controlSurface bombControl;// and the grad, dVel strips below
    void makeBombControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    // a laser
    laser theLaser;
    buttonValOnHit laserDamageStrip, laserAcc_tStrip;
    radioButton fceSelect;
    buttonRect beamLockButt;
    controlSurface laserControl;// and the grad, dVel strips below
    void makeLaserControl( sf::Vector2f hbPos, sf::Vector2f csPos );
//    bool doFireLaser = false;// Key = L, obviously

    // some ui
    buttonList shotControlList;// shot, shotBomb, clusterShot

    // motion control on paths
    controlSurface brakeControl;
    buttonRect pathButt[7];// vel+, vel-, brake, <-, ->, restart, kill
    buttonValOnHit cSpringStrip, dampStrip;
    sf::CircleShape pathCirc;// color indicates which path is controlled
    void makeBrakeControl( sf::Vector2f hbPos, sf::Vector2f csPos );
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
