#ifndef LVL_TRUCK_H_INCLUDED
#define LVL_TRUCK_H_INCLUDED

#include "Level.h"
#include "../scrollImage.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/buttonList.h"
#include "../button_types/controlSurface.h"
#include "../button_types/textButton.h"

class lvl_truck : public Level
{
    public:
    sf::CircleShape frontWheel, rearWheel;
    sf::CircleShape cmCirc;
    sf::Sprite bodySpr;
    sf::Texture wheelTxt, bodyTxt;
    sf::Vector2f cmPos;// center of mass = position of truck
    sf::Vector2f bodyOfst, fWheelOfst, rWheelOfst;// from cmPos

    sf::Vector2f rwBodyPos, fwBodyPos;// from cmPos = rwBodyPos- rWheelOfst
    float rwBodyVy = 0.0f, fwBodyVy = 0.0f;// body speed at wheel mounts
    float Ks = 2.5f, Kd = 1.0f;// shocks

    float bodyScale = 1.0f, wheelScale = 1.0f;
    float wRad = 20.0f;// wheel radius
    float fRotSpd = 0.0f, rRotSpd = 0.0f;// wheel rotation speeds
    float rotAccel = 0.0f;// wheelSlider assigns
    float brakeRate = 0.01f;
    bool braking = false;

    float Mass = 1000.0f;// in Kg?
    float fMass=1.0f, rMass=1.0f;// will treat as 2 point masses above wheels
    float baseLen = 1.0f;// fWheelOfst.x - rWheelOfst.x
    float gravityY = 0.15f;// in px/fr^2
    float wgt = Mass*gravityY, Fwgt = -wgt*rWheelOfst.x/( fWheelOfst.x - rWheelOfst.x );// front wheel weight

    scrollImage scrollBkgd;
    sf::Texture scrollTxt;

    // some ui

    slideBar wheelSlider;// torque control
 //   buttonRect brakeButt;
    buttonList brakeList;
    buttonValOnHit brakeStrip;// on list
    controlSurface truckControl;// and the grad, dVel strips below
    void makeTruckControl( sf::Vector2f hbPos, sf::Vector2f csPos );
    sf::Text weightMsg, weightMsgNum;
    sf::Text FweightMsg, FweightMsgNum;
    sf::Text RweightMsg, RweightMsgNum;
    sf::Text rotSpdMsg, rotSpdMsgNum;
    sf::Text roadSpdMsg, roadSpdMsgNum;

    // functions
    lvl_truck():Level() { std::cout << "Hello from lvl_truck ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~lvl_truck() { cleanup(); }
};

#endif // LVL_TRUCK_H_INCLUDED
