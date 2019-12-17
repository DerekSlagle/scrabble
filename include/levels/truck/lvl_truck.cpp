#include "lvl_truck.h"

bool lvl_truck::init()
{
    Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,0,0);
    button::setHoverBoxColor( Level::clearColor );

    std::ifstream fin("include/levels/truck/truck_init.txt");
    if( !fin ) { std::cout << "\nno truck_init"; return false; }
    std::string imgFname;
    fin >> imgFname;
    if( !bodyTxt.loadFromFile( imgFname.c_str() ) ) { std::cout << "no bodyTxt image data\n"; return false; }
    fin >> imgFname;
    if( !wheelTxt.loadFromFile( imgFname.c_str() ) ) { std::cout << "no wheelTxt image data\n"; return false; }

    fin >> bodyScale >> wheelScale;

    fin >> cmPos.x >> cmPos.y;
    fin >> bodyOfst.x >> bodyOfst.y;// upper left to cmPos
    fin >> fWheelOfst.x >> fWheelOfst.y;
    fin >> rWheelOfst.x >> rWheelOfst.y;
    fin >> wRad;
    std::cout << "\nwRad = " << wRad;
    rwBodyPos = cmPos + rWheelOfst;
    fwBodyPos = cmPos + fWheelOfst;

    bodySpr.setTexture( bodyTxt );
    bodySpr.setScale( bodyScale, bodyScale );
    bodySpr.setOrigin( bodyOfst/bodyScale );
//    bodySpr.setOrigin( bodyOfst );
    bodySpr.setPosition( cmPos );

    frontWheel.setTexture( &wheelTxt );
    frontWheel.setRadius( wRad );
    frontWheel.setOrigin( wRad, wRad );
    frontWheel.setPosition( fwBodyPos );
 //   frontWheel.setFillColor( sf::Color::Blue );
 //   frontWheel.setPosition( cmPos );

    rearWheel.setTexture( &wheelTxt );
    rearWheel.setRadius( wRad );
    rearWheel.setOrigin( wRad, wRad );
 //   rearWheel.setFillColor( sf::Color::Green );
    rearWheel.setPosition( rwBodyPos );

    cmCirc.setRadius( 10.0f );
    cmCirc.setOrigin( 10.0f, 10.0f );
    cmCirc.setPosition( cmPos );
    cmCirc.setFillColor( sf::Color::Yellow );

    fin >> Mass >> gravityY;
    baseLen = fWheelOfst.x - rWheelOfst.x;
    wgt = Mass*gravityY;
    Fwgt = -wgt*rWheelOfst.x/baseLen;// front wheel weight
    fMass = -Mass*rWheelOfst.x/baseLen;// front wheel mass
    rMass = Mass - fMass;

    fin >> Ks >> Kd;

    sf::Vector2f hbPos(10.0f,10.0f), csPos(70.0f,10.0f);
    makeTruckControl( hbPos, csPos );
    button::RegisteredButtVec.push_back( &truckControl );
    std::cout << "\nbrakeRate = " << brakeRate;

    // scrollImage for background
    scrollImage::rGR.width = Level::winW; scrollImage::rGR.height = Level::winH;
    scrollImage::rGR.left = 0.0f; scrollImage::rGR.top = 0.0f;
    if( !scrollTxt.loadFromFile("images/Level 4 BG SS.png") ) { std::cout << "no scroll image data\n"; return false; }
    scrollBkgd.INIT( scrollTxt, 6, 2564, 1737, 2251, 0.2f, 160.0f );


    return true;
}

bool lvl_truck::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_truck::update( float dt )
{
 /*   fRotSpd += wheelSlider.fVal;
        if( brakeButt.sel )
            {
                if( abs(fRotSpd) <= brakeRate )fRotSpd = 0.0f;
                else if( fRotSpd < 0.0f )fRotSpd += brakeRate;
                else fRotSpd -= brakeRate;
            }
        rRotSpd = fRotSpd;  */
    // new update body position here
    float dWgt = rotAccel*wRad*Mass*wRad/( fWheelOfst.x - rWheelOfst.x );
    to_SF_string( FweightMsgNum, Fwgt - dWgt );
    to_SF_string( RweightMsgNum, wgt - Fwgt + dWgt );
    float drY = -dWgt/Ks;
    float Len = fWheelOfst.x - rWheelOfst.x;
    float dcmY = drY*( fWheelOfst.x + rWheelOfst.x )/Len;
    cmPos = rearWheel.getPosition() - rWheelOfst;
    cmPos.y += dcmY;
    bodySpr.setPosition( cmPos );
    cmCirc.setPosition( cmPos );
    const float RtoD = 180.0f/3.1416f;
    bodySpr.setRotation( atan( 2.0f*RtoD*drY/Len) );
    // end new

    frontWheel.rotate( RtoD*fRotSpd*dt );
    rearWheel.rotate( RtoD*rRotSpd*dt );
    scrollBkgd.move();
    return;
}

void lvl_truck::draw( sf::RenderTarget& RT ) const
{
    scrollBkgd.draw(RT);
    RT.draw( bodySpr );
    RT.draw( frontWheel );
    RT.draw( rearWheel );
    RT.draw( cmCirc );
    return;
}

void lvl_truck::cleanup()
{
    return;
}

void lvl_truck::makeTruckControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    truckControl.init( hbPos, csPos, sf::Vector2f(400.0f, 110.0f), "Motion" );
    sf::Text label("rotSpeed", *button::pFont, 12);
    label.setColor( sf::Color::Black );
 //   const float RtoD = 180.0f/3.14159f;
    wheelSlider.init( csPos.x + 10.0f, csPos.y + 10.0f, 120.0f, 20.0f, label, -0.005f, 0.005f, 0.0f, nullptr );
 //   float wgt = Mass*gravityY, Fwgt = -wgt*rWheelOfst.x/( fWheelOfst.x - rWheelOfst.x );// front wheel weight
  /*  wheelSlider.pSetFunc = [this,Fwgt,wgt](float x)
    {
        rotAccel = x; to_SF_string( rotSpdMsgNum, fRotSpd );
        //               force   * r1/(r1+r2)
        float dWgt = x*wRad*Mass*wRad/( fWheelOfst.x - rWheelOfst.x );
        to_SF_string( FweightMsgNum, Fwgt - dWgt );
        to_SF_string( RweightMsgNum, wgt - Fwgt + dWgt );
    };  */
    wheelSlider.springBack = true;
    truckControl.pButtVec.push_back( &wheelSlider );
    label.setString("Brake");
 //   brakeButt.init( csPos.x + 10.0f, csPos.y + 60.0f, 60.0f, 40.0f, nullptr, label );
    brakeList.init( csPos.x + 10.0f, csPos.y + 60.0f, 50.0f, 30.0f, label );
    brakeList.pHitFunc = [this](){ braking = true; };
    brakeList.pHitFunc_up = [this](){ braking = false; };
    brakeList.persist = 2;
    brakeList.openOnRight = true;
    brakeList.pButtVec.push_back( &brakeStrip );
    label.setString("");
    brakeStrip.init( csPos.x + 70.0f, csPos.y + 70.0f, 100.0f, 20.0f, label, 0.001f, 0.01f, brakeRate, &brakeRate );
    brakeList.mode = 'I';
 //   brakeButt.mode = 'I';
 //   truckControl.pButtVec.push_back( &brakeButt );
    truckControl.pButtVec.push_back( &brakeList );

    weightMsg.setFont( *button::pFont );
    weightMsg.setCharacterSize( 16 );
    weightMsg.setColor( sf::Color::White );
    roadSpdMsg = roadSpdMsgNum = rotSpdMsg = rotSpdMsgNum = weightMsgNum = FweightMsg = FweightMsgNum = RweightMsg = RweightMsgNum = weightMsg;
    sf::Vector2f msgPos( csPos.x + 240.0f, csPos.y + 10.0f ), numPos( msgPos.x + 80.0f, msgPos.y );
    weightMsg.setString("Weight:");
    weightMsg.setPosition( msgPos );
 //   float wgt = Mass*gravityY;
    to_SF_string( weightMsgNum, wgt );
    weightMsgNum.setPosition( numPos );
    float dPosY = 18.0f;
    msgPos.y += dPosY; numPos.y += dPosY;
    FweightMsg.setString("front:");
    FweightMsg.setPosition( msgPos );
    to_SF_string( FweightMsgNum, Fwgt );
    FweightMsgNum.setPosition( numPos );
    msgPos.y += dPosY; numPos.y += dPosY;
    RweightMsg.setString("rear:");
    RweightMsg.setPosition( msgPos );
    to_SF_string( RweightMsgNum, wgt - Fwgt );
    RweightMsgNum.setPosition( numPos );
    msgPos.y += dPosY; numPos.y += dPosY;
    rotSpdMsg.setString("rotSpd:");
    rotSpdMsg.setPosition( msgPos );
    to_SF_string( rotSpdMsgNum, fRotSpd );
    rotSpdMsgNum.setPosition( numPos );
    msgPos.y += dPosY; numPos.y += dPosY;
    roadSpdMsg.setString("Speed:");
    roadSpdMsg.setPosition( msgPos );
    to_SF_string( roadSpdMsgNum, fRotSpd*wRad );
    roadSpdMsgNum.setPosition( numPos );
    truckControl.drawAll = [this](sf::RenderTarget& RT)
    {
        RT.draw( weightMsg );
        RT.draw( weightMsgNum );
        RT.draw( FweightMsg );
        RT.draw( FweightMsgNum );
        RT.draw( RweightMsg );
        RT.draw( RweightMsgNum );
        RT.draw( rotSpdMsg );
        RT.draw( rotSpdMsgNum );
        RT.draw( roadSpdMsg );
        RT.draw( roadSpdMsgNum );
    };

    truckControl.updateAll = [this]()
    {
        if( braking )
        {
            if( fRotSpd*fRotSpd <= brakeRate*brakeRate ){ fRotSpd = rRotSpd = 0.0f; rotAccel = 0.0f; to_SF_string( rotSpdMsgNum, 0.0f ); to_SF_string( roadSpdMsgNum, 0.0f ); scrollBkgd.vx = 0.0f; }
            else if( fRotSpd < 0.0f )rotAccel = brakeRate;
            else rotAccel = -brakeRate;
        }
        else rotAccel = wheelSlider.fVal;

        if( rotAccel != 0.0f )
        {
            fRotSpd += rotAccel;
            rRotSpd = fRotSpd;
            to_SF_string( rotSpdMsgNum, fRotSpd );
            to_SF_string( roadSpdMsgNum, fRotSpd*wRad );
            scrollBkgd.vx = -fRotSpd*wRad;
        }
    };
}

// non members
/*
void init_textList( buttonList& textList, float posx, float posy )
{
    sf::Text txt( "text", *button::pFont, 10);
    txt.setColor( sf::Color(0,0,0) );
    float wButt = 70.0f, hButt = 25.0f;
 //   waveControlList.init( posx, posy, 70.0f, 25.0f, txt );
    textList.init( posx, posy, wButt, hButt, txt );

    posy += hButt;
    txt.setString("text button 1");
    hButt = 30.0f; wButt = 150.0f;
    textButton* pTB = new textButton( posx, posy, wButt, hButt, txt ); pTB->maxCharCount = 15;
    pTB->pStrFunc = std::bind( [](const char* str) { std::cout << "\n#1 " << str; }, std::placeholders::_1 );
    textList.pButtVec.push_back( pTB );
    txt.setString("text button 2");
    pTB = new textButton( posx, posy+=hButt + 10.0f, wButt, hButt, txt ); pTB->maxCharCount = 15;
    pTB->pStrFunc = std::bind( [](const char* str) { std::cout << "\n#2 " << str; }, std::placeholders::_1 );
    textList.pButtVec.push_back( pTB );
    txt.setString("text button 3");
    pTB = new textButton( posx, posy+=hButt + 10.0f, wButt, hButt, txt ); pTB->maxCharCount = 15;
    pTB->pStrFunc = std::bind( [](const char* str) { std::cout  << "\n#3 " << str; }, std::placeholders::_1 );
    textList.pButtVec.push_back( pTB );

    textList.persist = 3;
    textList.ownButts = true;
}   */
