#ifndef LVL_TRACK_H_INCLUDED
#define LVL_TRACK_H_INCLUDED

#include "Level.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/buttonList.h"
#include "../button_types/controlSurface.h"
#include "../leg_types/legAdapter.h"
//#include "../spriteSheet.h"
#include "../pathUtils.h"

struct carAdapter
{
    legAdapter LA;
    sf::CircleShape circ;
    sf::Text tag;
    const Leg* pLaneChange = nullptr;
    float sLC = 0.0f;// s on other lane
    float tLC = 0.0f, tLC_total = 20.0f;// lane change time
    float vFull = 0.0f, acc = 0.5f;// LA.v may vary if must wait to pass
    bool waitToPass = false;// =true in lvl_track::update(), =false in carAdapter::update()
    void init(  Leg* p_Leg, float Radius, const sf::Text& Tag, sf::Color clr, float vel = 0.0f );
    void draw( sf::RenderTarget& RT ) const { RT.draw( circ ); RT.draw( tag ); }
    void update( float dt );
    carAdapter( const carAdapter& rCA );//: LA( rCA.LA ) { LA.pSetPosition = [this](float x, float y){ circ.setPosition(x,y); tag.setPosition(x,y); }; }
    carAdapter(): LA(){}
    const Leg* findAdjacentLane( Leg** pPath ) const;// assign to pLaneChange if usefull
    void makeLaneChange( const Leg* p_LaneChange, Leg** pPath );
};

class lvl_track : public Level
{
    public:
    // main path motion
    Leg* pPath[2] = { nullptr, nullptr };// inner, outer lanes
    std::vector<sf::Vertex> pathVtxVec[2];// linesStrip showing path plot each lane

    std::vector<carAdapter> carAdaptVec;// [0]=car'A', [1]=car'B'
    const float carRadius = 6.0f;
    std::vector<sf::CircleShape> wtpIndicatorVec, slowIndicatorVec;// drawn each car to show state

    // some ui
    buttonRect goButt;// start/stop motion
    buttonRect badLaneButt;// deny lane change while held
    buttonRect passTestButt;// setup pass condition on cars this=0, slow=1, obstruct=2
    buttonRect tryAutoButt;// setup pass condition on cars this=0, slow=1, obstruct=2

    controlSurface trackControl;// speed and lane change controls for cars 'A' and 'B'
    buttonRect laneButt1, laneButt2;// force lane change for car 'A' or 'B'
    multiSelector trackMS;
    buttonValOnHit trackStrip;
 //   buttonValOnHit speedStrip1, speedStrip2, tLaneChangeStrip;
 //   buttonValOnHit accStrip;
    void makeTrackControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    // functions
    lvl_track():Level() { std::cout << "Hello from lvl_track ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void cleanup();
    virtual ~lvl_track() { cleanup(); }
    // this level
 //   bool changeLane( carAdapter& rCA );
    bool checkLane( carAdapter& rCA, const Leg* pLC );
};

#endif // LVL_TRACK_H_INCLUDED
