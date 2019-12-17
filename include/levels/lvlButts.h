#ifndef LVLBUTTS_H_INCLUDED
#define LVLBUTTS_H_INCLUDED

#include "Level.h"
// established types
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"
#include "../button_types/slideBar.h"
#include "../button_types/buttonList.h"
#include "../button_types/textButton.h"
#include "../button_types/buttStrip.h"
// new types
#include "../button_types/buttonImg.h"
#include "../button_types/buttonTri.h"
#include "../button_types/buttonSlip.h"
#include "../button_types/slideBar.h"
// other
#include "../spriteSheet.h"
#include "../pathUtils.h"
#include "../leg_types/linLeg.h"
#include "../dialDisplay.h"

class lvlButts : public Level
{
    public:
    buttonTri triButt;
    buttonImg LatchButt1;
    spriteSheet SSbuttLatch;

    buttonImg onOffButt;
    spriteSheet onOffButtSS;

    textButton msgButt;
    sf::Text buttMsg;
    buttonList txtButtList;

    dialDisplay accDial, velDial, posDial;
    sf::Texture dialTxt, needleTxt;
    slideBar dialSlide;
    buttStrip fudgeBS;
    buttonRect brakeButt;
    buttonRect homeButt;
    float brakeRate = 0.1f;
    controlSurface motionControl;

    buttonSlip slipButt1;
    spriteSheet SSbuttSlip;

    // motion
    //              moves    marks home  or overshoot home
    sf::CircleShape moveCirc, stayCirc, stopPtCirc;
    sf::Text stopDistMsg, stopDistPxMsg;// shown from stop at home until next movement
    bool atHome = false;

    // functions
    lvlButts():Level() { std::cout << "Hello from lvButts ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~lvlButts() { cleanup(); }
};

void init_textList( buttonList& textList, float posx, float posy );

#endif // LVLBUTTS_H_INCLUDED
