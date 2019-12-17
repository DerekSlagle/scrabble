#ifndef LVL_PERSPECT_H_INCLUDED
#define LVL_PERSPECT_H_INCLUDED

#include "Level.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonList.h"
#include "../utility/vec2f.h"
#include "../utility/perspectivePoint.h"
#include "../spriteSheet.h"

class lvl_perspect : public Level
{
    public:
    sf::Vertex horizonLine[2], groundLine[2], vanishPt[2];// xHair intersect
    std::vector<persPt> persPtVec;
    std::vector<sf::Vertex> vtxVec;
    float Xv = 0.0f, moveSpeedX = 5.0f;
    int moveX = 0;// movementdirection
    float Zv = 0.0f, moveSpeedZ = 4.0f;
    int moveZ = 0;// movementdirection
    vec3f viewPos;// viewer position
    vec3f gravity;

    persBox boxA;
    std::vector<persBox> boxVec;
    spriteSheet faceSS;

    persBoxMover flyBox;// fly over scene
    vec3f flyIpos, flyFpos;
    float flySpeed = 3.0f;

    // scene markings
    persPt road[2];// at road edges
    sf::Vertex roadVtx[3];
    persPt edgeLine[4];// left and right from persPt::Yh + dy0 to persPt::Yg
    sf::Vertex edgeVtx[4];
    persPt sideWall[8];// at xs = +-xMax, +-yMax, Yf+dYf to Yg
    sf::Vertex wallVtx[12];// 3 x sf::Quad = left, right, back walls
    sf::Vertex floorCeilingVtx[8];// 2 x sf::Quad
    float xMax = 1000.0f, yMax = 500.0f, dYh = 10.0f;// dYf = distance from back wall to horizon line
    float roadWidth = 200.0f;
    void makeBounds();

    // some ui
    buttonValOnHit boxMsStrip;
    multiSelector boxMS;
    controlSurface boxControl;
    buttonRect cloneButt;
    bool makeBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos );
 //   buttonValOnHit YgStrip;


    buttonValOnHit enviroMsStrip;
    multiSelector enviroMS;// Yg, Yh, xMax, Ymax, dYh
    controlSurface enviroControl;
    buttonRect moveZbutt, moveXLtButt, moveXRtButt;// belt on/off, etc
    bool makeEnviroControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    buttonValOnHit flyBoxMsStrip;
    multiSelector flyBoxMS;
    controlSurface flyBoxControl;
    sf::Text posZmsg, posZnumMsg;
    bool makeFlyBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    controlSurface otherControl;// save, load, gravity?
    buttonRect saveToFileButt, loadFromFileButt;
    bool makeOtherControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    buttonList controlList;// box, fly, enviro

    // functions
    lvl_perspect():Level() { std::cout << "Hello from lvl_perspect ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void cleanup(){}
    virtual ~lvl_perspect() { cleanup(); }
};

#endif // LVL_PERSPECT_H_INCLUDED
