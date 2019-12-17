#include "lvl_truck.h"

const float RtoD = 180.0f/3.1416f;

 void transmission::init( std::istream& is )
 {
    is >> reverseIO;// ratio in reverse gear ( -1.0f )
    is >> gearNum;// abuse as number of gears

    gearVec.reserve( gearNum );
    for( int i = 0; i < gearNum; ++i )
    {
        gear tmpGear;
        is >> tmpGear.ratioIO >> tmpGear.spdDn >> tmpGear.spdUp;
        gearVec.push_back( tmpGear );
    }

    gearNum = 1;// Neutral or Parked
 }

void transmission::setOutRotSpd( float rotSpd )
{
    outRotSpd = rotSpd;
    if( gearNum == 0 ) return;// neautral

    if( rotSpd < 0.0f )
    {
        gearNum = -1;// reverse
        inRotSpd = outRotSpd*reverseIO;// reverse
    }
    else// gearNum > 0
    {
        if( gearNum == -1 ) gearNum = 1;// reverse to drive
        while( outRotSpd*gearVec[gearNum-1].ratioIO > gearVec[gearNum-1].spdUp && gearNum < (int)gearVec.size() ) ++gearNum;// up shift
        while( outRotSpd*gearVec[gearNum-1].ratioIO < gearVec[gearNum-1].spdDn && gearNum > 1 ) --gearNum;// down shift
        inRotSpd = outRotSpd*gearVec[gearNum-1].ratioIO;
    }
}

void transmission::setInRotSpd( float rotSpd )
{
    inRotSpd = rotSpd;
    if( gearNum == 0 ) return;// neautral

    if( rotSpd < 0.0f )
    {
        gearNum = -1;// reverse
        outRotSpd = inRotSpd/reverseIO;// reverse
    }
    else// gearNum > 0
    {
        if( gearNum == -1 ) gearNum = 1;// reverse to drive
        while( inRotSpd/gearVec[gearNum-1].ratioIO > gearVec[gearNum-1].spdUp && gearNum < (int)gearVec.size() ) ++gearNum;// up shift
        while( inRotSpd/gearVec[gearNum-1].ratioIO < gearVec[gearNum-1].spdDn && gearNum > 1 ) --gearNum;// down shift
        outRotSpd = inRotSpd/gearVec[gearNum-1].ratioIO;
    }
}

// level_truck member functions
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

    fin >> Mass >> gravityY;
    baseLen = fWheelOfst.x - rWheelOfst.x;
    wgt = Mass*gravityY;
    Fwgt = -wgt*rWheelOfst.x/baseLen;// front wheel weight
    fMass = -Mass*rWheelOfst.x/baseLen;// front wheel mass
    rMass = Mass - fMass;
    std::cout << "\nwgt = " << wgt << "Fwgt = " << Fwgt << " Rwgt = " << wgt - Fwgt;

    fin >> Ks >> Kd;
    fin >> faceDir;
    fin.close();

    fin.open("include/levels/truck/xmission_init.txt");
    if( !fin ) { std::cout << "\nno xmission_data"; return false; }
    truckXmsn.init( fin );
    fin.close();
    gearMsg.setFont( *button::pFont );
    gearMsg.setCharacterSize(20);
    gearMsg.setColor( sf::Color::Red );
    gearMsg.setString( "0" );
    gearMsg.setPosition( 1000.0f, 20.0f );
    rpmMsg = gearLabel = gearMsg;
    rpmMsg.setPosition( 1000.0f, 50.0f );
    gearLabel.setString( "Gear:");

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
    frontWheel.setPosition( fwBodyPos.x, fwBodyPos.y - Fwgt/Ks );
 //   frontWheel.setFillColor( sf::Color::Blue );
 //   frontWheel.setPosition( cmPos );

    rearWheel.setTexture( &wheelTxt );
    rearWheel.setRadius( wRad );
    rearWheel.setOrigin( wRad, wRad );
 //   rearWheel.setFillColor( sf::Color::Green );
    rearWheel.setPosition( rwBodyPos.x, rwBodyPos.y - (wgt-Fwgt)/Ks );
 //   rearWheel.setPosition( rwBodyPos.x, rwBodyPos.y );// -Fwgt/Ks );

    cmCirc.setRadius( 4.0f );
    cmCirc.setOrigin( 4.0f, 4.0f );
    cmCirc.setPosition( cmPos );
    cmCirc.setFillColor( sf::Color::White );

    speedDial.pDialTxt = &dialTxt;
    speedDial.pNeedleTxt = &needleTxt;
    if( !speedDial.init( sf::Vector2f( 700.0f, 200.0f ), "include/levels/truck/dialDisplay_init.txt" ) ) { std::cout << "\nno dial init"; return false; }
 //   if( !accelDial.init( sf::Vector2f( 700.0f, 200.0f ), "include/levels/truck/dialDisplayAccel_init.txt" ) ) { std::cout << "\nno dial init"; return false; }


    sf::Vector2f hbPos(10.0f,10.0f), csPos(70.0f,50.0f);
    makeTruckControl( hbPos, csPos );
    button::RegisteredButtVec.push_back( &truckControl );
    std::cout << "\nbrakeRate = " << brakeRate;

    // scrollImage for background
    scrollImage::rGR.width = Level::winW; scrollImage::rGR.height = Level::winH;
    scrollImage::rGR.left = 0.0f; scrollImage::rGR.top = 0.0f;
    BkgdArr::rGR.width = Level::winW; BkgdArr::rGR.height = Level::winH;
    BkgdArr::rGR.left = 0.0f; BkgdArr::rGR.top = 0.0f;

    if( !scrollTxt.loadFromFile("images/Level 4 BG SS.png") ) { std::cout << "no scroll image data\n"; return false; }
    scrollBkgd.INIT( scrollTxt, 6, 2564, 1737, 2251, 0.3f, 100.0f );
    scrollBK_fg.INIT( scrollTxt, 6, 2564, 2314, 2562, 1.0f, 0.0f );

    if( !palmTxt.loadFromFile("images/cars/palmTreeA.png") ) { std::cout << "no palm tree image data\n"; return false; }
    fin.open("images/cars/palmTreeA_data.txt");
    if( !fin ) { std::cout << "\nno palmTreeA_data"; return false; }
    float scale; fin >> scale;
    palmStrip.init( palmTxt, scale, fin );
    fin.close();

    if( !cloudTxt.loadFromFile("images/cars/cloudsx4.png") ) { std::cout << "no cloud image data\n"; return false; }
    fin.open("images/cars/cloudsx4_data.txt");
    if( !fin ) { std::cout << "\nno cloudsx4_data"; return false; }
    fin >> scale;
    cloudStrip.init( cloudTxt, scale, fin );
    fin.close();

    // sound
    if ( !engineStartSndBuff.loadFromFile( "images/cars/car+start3.wav" ) ) { std::cout << "no engine start sound\n"; return false; }
    engineStartSnd.setBuffer( engineStartSndBuff );
    engineStartSnd.setVolume( 50.0f );
    if ( !engineRunSndBuff.loadFromFile( "images/cars/airplane+spita.wav" ) ) { std::cout << "no engine run sound\n"; return false; }
    engineRunSnd.setBuffer( engineRunSndBuff );
    engineRunSnd.setVolume( 50.0f );
    engineRunSnd.setLoop( true );

 /*   sf::Text label("Start", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    engineRunButt.init( 40.0f, Level::winH - 60.0f, 50.0f, 30.0f, nullptr, label );
     engineRunButt.pHitFunc = [this]()
     {
        if( engineRunButt.sel )
            engineStartSnd.play();
        else
        {
            if( engineStartSnd.getStatus() == sf::SoundSource::Playing ) engineStartSnd.stop();
            else if( engineRunSnd.getStatus() == sf::SoundSource::Playing ){  fadePitch = true;  std::cout << "Engine stopping..."; }
        }
     };
    button::RegisteredButtVec.push_back( &engineRunButt );

//    std::function<sf::Sound*(void)> pPlayIDfunc = [this]()
  //  {
    //    sf::Sound* pSnd = nullptr;
//        if( engineStartSound.getStatus == sf::SoundSource::Playing ) pSnd = &engineStartSound;
  //      else if( engineRunSound.getStatus == sf::SoundSource::Playing ) pSnd = &engineRunSound;
    //    return pSnd;
//    };
    label.setString("pitch");
    pitchSlide.init( 300.0f, Level::winH - 60.0f, 120.0f, 20.0f, label, 0.4f, 3.0f, 0.7f, [this](float x){ engineRunSnd.setPitch(x); } );
    pitchSlide.tuneFactor = 0.01f;
    button::RegisteredButtVec.push_back( &pitchSlide );
*/
    sf::Text label("volume", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    volSlide.init( 400.0f, Level::winH - 50.0f, 120.0f, 20.0f, label, 0.0f, 100.0f, 50.0f, [this](float x){ engineStartSnd.setVolume(x); engineRunSnd.setVolume(x); } );
    button::RegisteredButtVec.push_back( &volSlide );

    return true;
}

bool lvl_truck::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_truck::update( float dt )
{
    static sf::SoundSource::Status lastStartStatus = sf::SoundSource::Stopped;// Paused, Playing = other enums
    sf::SoundSource::Status currStartStatus = engineStartSnd.getStatus();
    if( lastStartStatus != currStartStatus )// watch for sound events
    {
        if( currStartStatus == sf::SoundSource::Playing ) std::cout << "\nEngine starting...";
        else if( currStartStatus == sf::SoundSource::Stopped && engineRunButt.sel )// starter just stopped. Run engine
        {
            std::cout << "Starter stopped\nengine running...";
            pitchSlide.set_fVal( 0.7f );
            rpmDial.setValue(0.7f);
            engineRunSnd.play();
        }

        lastStartStatus = currStartStatus;
    }

    if( fadePitch )
    {
        if( engineRunSnd.getStatus() == sf::SoundSource::Playing )
        {
            float currPitch = engineRunSnd.getPitch();
            if( currPitch > minPitch )
            {
                pitchSlide.set_fVal( currPitch - fadeRate );
                rpmDial.setValue( pitchSlide.fVal );
            //    engineRunSnd.setPitch( currPitch - fadeRate );
            }
            else
            {
                engineRunSnd.stop();
                rpmDial.setValue(0.0f);
                fadePitch = false;
                std::cout << "Engine stopped";
            }
        }
    }



    if( truckControl.sel )
    {
        // new update body position here
        float dWgt = rotAccel*wRad*Mass*wRad/baseLen;
        float FwgtNet = Fwgt - dWgt;
        float RwgtNet = wgt - Fwgt + dWgt;
        to_SF_string( FweightMsgNum, FwgtNet );
        to_SF_string( RweightMsgNum, RwgtNet );
        float drY = -dWgt/Ks;
        rwBodyPos.y += rwBodyVy*dt;
        rwBodyVy += ( Ks*( rearWheel.getPosition().y - rwBodyPos.y ) - Kd*rwBodyVy + RwgtNet )*dt/rMass;
        fwBodyPos.y += fwBodyVy*dt;
        fwBodyVy += ( Ks*( frontWheel.getPosition().y - fwBodyPos.y ) - Kd*fwBodyVy + FwgtNet )*dt/fMass;
        cmPos = rwBodyPos - rWheelOfst; cmPos.y += drY;
        bodySpr.setPosition( cmPos );
        cmCirc.setPosition( cmPos );
        bodySpr.setRotation( RtoD*atan( ( fwBodyPos.y - rwBodyPos.y )/baseLen) );

    }
    // end new

    frontWheel.rotate( RtoD*fRotSpd*dt );
    rearWheel.rotate( RtoD*rRotSpd*dt );

    scrollBK_fg.move();
    scrollBkgd.move();
    cloudStrip.update(dt);
    palmStrip.update(dt);
    return;
}

void lvl_truck::draw( sf::RenderTarget& RT ) const
{
    cloudStrip.draw(RT);
    scrollBkgd.draw(RT);
    palmStrip.draw(RT);
    scrollBK_fg.draw(RT);
    RT.draw( bodySpr );
    RT.draw( frontWheel );
    RT.draw( rearWheel );
    RT.draw( cmCirc );
    RT.draw( rpmMsg );
    return;
}

void lvl_truck::cleanup()
{
    return;
}

void lvl_truck::makeTruckControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    truckControl.init( hbPos, csPos, sf::Vector2f(840.0f, 110.0f), "Motion" );
    sf::Text label("accel", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    float maxAcc = 0.008f/truckXmsn.gearVec[0].ratioIO;
    wheelSlider.init( csPos.x + 10.0f, csPos.y + 10.0f, 120.0f, 20.0f, label, -maxAcc, maxAcc, 0.0f, [this](float x){ if( abs(fRotSpd*wRad) > speedDial.maxVal ) wheelSlider.setSel(false); } );

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
    truckControl.pButtVec.push_back( &brakeList );

    weightMsg.setFont( *button::pFont );
    weightMsg.setCharacterSize( 16 );
    weightMsg.setColor( sf::Color::White );
    roadSpdMsg = roadSpdMsgNum = rotSpdMsg = rotSpdMsgNum = weightMsgNum = FweightMsg = FweightMsgNum = RweightMsg = RweightMsgNum = weightMsg;
    sf::Vector2f msgPos( csPos.x + 190.0f, csPos.y + 10.0f ), numPos( msgPos.x + 70.0f, msgPos.y );
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

    gearLabel.setPosition( csPos.x + 400.0f, csPos.y + 70.0f );
    gearMsg.setPosition( csPos.x + 470.0f, csPos.y + 70.0f );

    label.setString("Speed"); speedDial.label = label; speedDial.label.setColor( sf::Color::Red );
    speedDial.setPosition( csPos.x + 400.0f, csPos.y + 20.0f );
    accelDial = rpmDial = speedDial;

    label.setString("Engine RPM"); rpmDial.label = label; rpmDial.label.setColor( sf::Color::Red );
    rpmDial.minVal = 0.0f; rpmDial.maxVal = 4.0f;
    rpmDial.setPosition( csPos.x + 500.0f, csPos.y + 20.0f );

    label.setString("Accel"); accelDial.label = label; accelDial.label.setColor( sf::Color::Red );
    accelDial.minVal = -0.008f; accelDial.maxVal = 0.008f;
    accelDial.setPosition( csPos.x + 300.0f, csPos.y + 20.0f );

    // sound
    label.setString("START");
    engineRunButt.init( csPos.x + 590.0f, csPos.y + 10.0f, 50.0f, 30.0f, nullptr, label );
    engineRunButt.pHitFunc = [this]()
    {
        if( engineRunButt.sel )
        {
            engineStartSnd.play();
            rpmDial.setValue( 0.2f );
        }
        else
        {
            if( engineStartSnd.getStatus() == sf::SoundSource::Playing ) { engineStartSnd.stop(); rpmDial.setValue( 0.0f ); }
            else if( engineRunSnd.getStatus() == sf::SoundSource::Playing ){  fadePitch = true;  std::cout << "Engine stopping..."; }
        }
    };
    truckControl.pButtVec.push_back( &engineRunButt );

    label.setString("pitch");
    pitchSlide.init( csPos.x + 650.0f, csPos.y + 10.0f, 120.0f, 20.0f, label, 0.4f, 4.0f, 0.7f, [this](float x){ engineRunSnd.setPitch(x); } );
    pitchSlide.tuneFactor = 0.01f;
    truckControl.pButtVec.push_back( &pitchSlide );

    // for objects on this surface
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
        speedDial.draw(RT);
        accelDial.draw(RT);
        rpmDial.draw(RT);
        RT.draw( gearMsg );
        RT.draw( gearLabel );
    };

    truckControl.updateAll = [this]() { truckControl_update(); };
}

// helper is called via truckControl.updateAll
void lvl_truck::truckControl_update()
{
    if( braking )
        {
            if( fRotSpd*fRotSpd <= brakeRate*brakeRate )
            {
                fRotSpd = rRotSpd = 0.0f; rotAccel = 0.0f;
                to_SF_string( rotSpdMsgNum, 0.0f ); to_SF_string( roadSpdMsgNum, 0.0f );
                scrollBkgd.vx = 0.0f;
                scrollBK_fg.vx = 0.0f;
                palmStrip.vx = 0.0f;
                cloudStrip.vx = 0.0f;
                speedDial.setValue( 0.0f );
                accelDial.setValue( 0.0f );
                pitchSlide.hitRight();// reset to iVal
                rpmDial.setValue( pitchSlide.iVal );
                truckXmsn.setOutRotSpd( 0.0f );
                to_SF_string( gearMsg, truckXmsn.gearNum );
                to_SF_string( rpmMsg, truckXmsn.inRotSpd );
            }
            else if( fRotSpd < 0.0f )rotAccel = brakeRate;
            else rotAccel = -brakeRate;
        }
        else
        {
            rotAccel = wheelSlider.fVal* truckXmsn.getGearRatio();
            if( truckXmsn.gearNum == -1 ) rotAccel *= -1.0f;// reverse
        }

   //     static int gearLast = 0;
   //     if( truckXmsn.gearNum != gearLast )// on gear change
   //     {
   //         std::cout << "\nGear: " << truckXmsn.gearNum << " ratio: " << truckXmsn.getGearRatio() << " rotAccel: : " << rotAccel;
   //         gearLast = truckXmsn.gearNum;
    //    }

        accelDial.setValue( rotAccel );

        if( rotAccel != 0.0f )
        {
            fRotSpd += rotAccel;
            rRotSpd = fRotSpd;
            truckXmsn.setOutRotSpd( faceDir == 'R' ? fRotSpd : -fRotSpd );
            if( truckXmsn.outRotSpd < 0.0f ) gearMsg.setString("Rev");
            else to_SF_string( gearMsg, truckXmsn.gearNum );
            to_SF_string( rpmMsg, truckXmsn.inRotSpd );
            to_SF_string( rotSpdMsgNum, fRotSpd );
            to_SF_string( roadSpdMsgNum, fRotSpd*wRad );
            scrollBK_fg.vx = -fRotSpd*wRad;
            scrollBkgd.vx = scrollBkgd.vFactor*scrollBK_fg.vx;
            palmStrip.vx = palmStrip.vFactor*scrollBK_fg.vx;
            cloudStrip.vx = cloudStrip.vFactor*scrollBK_fg.vx;
            speedDial.setValue( abs( scrollBK_fg.vx ) );
         //   pitchSlide.set_fVal( pitchSlide.iVal + ( pitchSlide.fVal_max - pitchSlide.iVal )*truckXmsn.inRotSpd/(speedDial.maxVal/wRad) );
            pitchSlide.set_fVal( pitchSlide.iVal + ( 3.0f - pitchSlide.iVal )*truckXmsn.inRotSpd/(speedDial.maxVal/wRad) );
            rpmDial.setValue( pitchSlide.fVal );
      //      pitchSlide.set_fVal( pitchSlide.iVal + ( pitchSlide.fVal_max - pitchSlide.iVal )*speedDial.currVal/speedDial.maxVal );
        }
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
