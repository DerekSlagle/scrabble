#ifndef LVL_PATHS_H_INCLUDED
#define LVL_PATHS_H_INCLUDED

#include "Level.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/buttonList.h"
#include "../button_types/controlSurface.h"
#include "../leg_types/legAdapter.h"
//#include "../spriteSheet.h"
#include "../pathUtils.h"
#include "../graphFuncs.h"

class lvl_paths : public Level
{
    public:

    // main path motion
    Leg* pPath = nullptr;
    float v1 = 2.0f;// launch speed for pPath
    std::vector<sf::Vertex> pathVtxVec;// linesStrip showing path plot
    controlSurface pathControl;
    buttonValOnHit speedStrip;// for pathControl
    multiSelector pathMS;
    buttonValOnHit accelStrip;// for pathMS

    // for base path in compound motion motion
    Leg* pPath_2 = nullptr;
    float v2;// launch speed on path 2
    controlSurface pathControl_2;
    buttonValOnHit speedStrip_2, posStrip_2;// start speed and position (s value)
    multiSelector pathMS_2;
    buttonValOnHit accelStrip_2;// for pathMS
    buttonRect useP2Butt;// toggle usage
    legAdapter legAdapt_2;// pointed to by pBaseLA member of legAdapter for compuond motion
    sf::CircleShape circ_2;// used by la above

    // path travelers
    std::vector<inertialAdapter> inertAdaptVec;// adapter object movable + path motion
    std::vector<sf::CircleShape> circVec;// for inertAdaptVec
    sf::Texture ballTxtB;// for above balls
    sf::CircleShape circ;// template for vector of balls construction. See reInit()
    // colliders - 1 in use
    std::vector<inertialAdapter> freeAdaptVec;// adapter object movable + path motion
    std::vector<sf::CircleShape> freeCircVec;// for freeAdaptVec
    sf::Texture ballTxtA;// for above balls

    // graphFuncs
    polygonIrreg pgIrreg;
    graphFuncs gfPolyIR;
    graphFuncs gfPolygon;
    polygonReg pgReg;
    graphFuncs* pGfunc = nullptr;
    // control for the graphFunc
    controlSurface gfCsurf;
    multiSelector gfMs;
    buttonValOnHit gfVoh;
    slideBar gfSlider;

    bool anyMoving = false;
    sf::Vector2f gravity = sf::Vector2f(0.0f,0.0f);

    //*** controls **
    // motion control buttons
    buttonRect startButt;// re initialize path and start motion
    buttonRect hitButt;// toggle hit detection
    buttonRect brakeButt, accelButt, decelButt, allOr1Butt;// control motion of all or just 1st in vector
    buttonList pathList;// with sub lists for normal and function type paths

    // inertia properties control: spring stiffness, damping, mass, gravity
    buttonValOnHit inertiaStrip;// for inertia MS
    multiSelector inertiaMS;
    float Cspring, Cdamp, Mass;// for inertiaMS to write to. Easy reference in reInit()
 //   buttonValOnHit springStrip, dampStrip, massStrip, gravityStrip;// for inertia control
    controlSurface inertiaControl;

    // launch control: how many, launch delay
    buttonValOnHit numStrip, delayStrip;// for launchControl
    controlSurface launchControl;// number and delay

    buttonList controlList;// for the controls: launchControl, inertiaControl, pathControl, pathControl_2

    // functions
    lvl_paths():Level() { std::cout << "Hello from lvl_paths ctor\n"; }

    virtual bool init();
    bool reInit();// from file data
    bool loadNewPath( const char* fileName, char type );// type = 'N'=normal, 'F'=function

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~lvl_paths() { cleanup(); }

    void makePathControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    void makePathControl_2( sf::Vector2f hbPos, sf::Vector2f csPos );// for base path
    void makeInertiaControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    void makeLaunchControl( sf::Vector2f hbPos, sf::Vector2f csPos );
};

#endif // LVL_PATHS_H_INCLUDED
