#ifndef LVL_PERSPECT_H_INCLUDED
#define LVL_PERSPECT_H_INCLUDED

#include<algorithm>
#include "Level.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonList.h"
#include "../button_types/joyButton.h"
#include "../utility/vec2f.h"
#include "../utility/perspectivePoint.h"
#include "../spriteSheet.h"
#include "../leg_types/legAdapter.h"
#include "../pathUtils.h"

class lvl_perspect : public Level
{
    public:
    sf::Vertex horizonLine[2], vanishPt[2];// xHair intersect
 //   sf::Vertex groundLine[2];// xHair intersect
    float moveSpeedX = 5.0f;
    int moveX = 0;// movementdirection
    float Zv = 0.0f, moveSpeedZ = 4.0f;
    int moveZ = 0;// movementdirection
    float moveSpeedY = 4.0f;
    float moveSens = 0.2f;// for scaling fly_jButt input. assign with enviroMS
    int moveY = 0;
    vec3f gravity;
    float dAngPolar = 0.004f;// enviroMS. Rotate a persMatrix

    persBox boxA;// the van
    legAdapter LAboxA;
    Leg* pLegTrack[2] = { nullptr, nullptr };// 2 lanes
  //  std::vector<sf::Vertex> laneVtxVec[2];
    std::vector<persBox> boxVec;// buildings, etc. from savedBuildings.txt
    std::vector<spriteSheet> SSvec;// treeTxt, trackTxt, faceSS, tileSS, blockHouses, historicBuilding, mansion
    bool loadSpriteSheets();

    // more boxes = cars
    std::vector<persBox> carVec;
    std::vector<legAdapter> carLAvec;

    persBoxMover flyBox;// fly over scene
    vec3f flyIpos, flyFpos;
    float flySpeed = 3.0f;

    std::vector<persQuad> persQuadVec;// stationary
    std::vector<persQuadAni> persQuadAniVec;// stationary
    std::vector<persMatrix> persMatVec;// toRep raw persPtVec
    // all persPack types. For drawing from.
    std::vector<persPack*> ppPackVec;// pointers to drawn objects sorted by distance from persPack::viewPt
    // animate a quad

    // scene markings
    persQuad floorQuad;
    sf::Texture floorTxt;
//    persPt road[2];// at road edges
//    sf::Vertex roadVtx[3];
    persPt edgeLine[4];// left and right from persPt::Yh + dy0 to persPt::Yg
    sf::Vertex edgeVtx[4];
    persPt sideWall[8];// at xs = +-xMax, +-yMax, Yf+dYf to Yg
    sf::Vertex wallVtx[12];// 3 x sf::Quad = left, right, back walls
    sf::Vertex floorCeilingVtx[8];// 2 x sf::Quad
    persPt track[4];
    sf::Vertex trackQuad[4];
    persQuad trackPQ;// toRep above 2 members
 //   sf::Texture trackTxt, treeTxt;
    vec3f trackPos;
    float trackDimX = 800.0f, trackDimZ = 600.0f;
    float xMax = 1000.0f, yMax = 500.0f, dYh = 50.0f;// dYf = distance from back wall to horizon line
    float roadWidth = 200.0f;
    void makeBounds();

    // some ui
    buttonValOnHit boxMsStrip;
    multiSelector boxMS;
    controlSurface boxControl;
    buttonRect cloneButt, pathButt;
    bool makeBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    buttonValOnHit enviroMsStrip;
    multiSelector enviroMS;// Yg, Yh, xMax, Ymax, dYh
    sf::Text XvTxt, YvTxt, ZvTxt;
    controlSurface enviroControl;
    buttonRect moveLtButt, moveRtButt;// scroll view
    joyButton fly_jButt;// scroll x-z. mseLt/Rt = scroll Up/Dn
    bool makeEnviroControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    buttonValOnHit flyBoxMsStrip;
    multiSelector flyBoxMS;
    controlSurface flyBoxControl;
    sf::Text posXmsg, posYmsg, posZmsg;
    bool makeFlyBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos );

 /*   buttonValOnHit gunMsStrip;
    multiSelector gunMS;
    controlSurface gunControl;
    persBox gunBox;
    spriteSheet gunSS;
    bool makeGunControl( sf::Vector2f hbPos, sf::Vector2f csPos );  */

    controlSurface otherControl;// save, load, gravity?
    buttonRect saveToFileButt, loadFromFileButt;
    bool makeOtherControl( sf::Vector2f hbPos, sf::Vector2f csPos );

    buttonList controlList;// box, fly, enviro, other

    // functions
    lvl_perspect():Level() { std::cout << "Hello from lvl_perspect ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void cleanup();
    virtual ~lvl_perspect() { cleanup(); }
};

void setTxtRect( sf::Vertex* vtx, sf::IntRect srcRect );
void updateLAbox( legAdapter& rLA, persBox& rCar, float dt );

#endif // LVL_PERSPECT_H_INCLUDED
