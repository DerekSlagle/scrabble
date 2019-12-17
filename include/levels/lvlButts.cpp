#include "lvlButts.h"

bool lvlButts::init()
{
   Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,50,0);
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;

    sf::Text label("TRI", *button::pFont, 12);
    label.setColor( sf::Color::Black );
 //   label.setString("TRI");
    sf::Vertex Pt[3]; Pt[0].position = sf::Vector2f(0.0f,50.0f); Pt[1].position = sf::Vector2f(0.0f,0.0f);  Pt[2].position = sf::Vector2f(35.0f,25.0f);
    sf::Color clr[4]; clr[0] = sf::Color::Blue; clr[2] = sf::Color::Green; clr[1] = sf::Color::Red; clr[3] = sf::Color::Yellow;
    triButt.init( Pt, 100.0f, 100.0f, clr, label );
    button::RegisteredButtVec.push_back( &triButt );

    // load a spriteSheet
    std::ifstream inFile("images/buttonsLatchingSSdata.txt");
    if( !inFile ) return false;
    SSbuttLatch.init( inFile );
    inFile.close();
    sf::RectangleShape Rinit[4];
    SSbuttLatch.copySet( Rinit, 0 );// 3 sets
    label.setString("Latch");
    LatchButt1.init( Rinit, 100.0f, 200.0f, label );
    LatchButt1.mode = 'L';
    button::RegisteredButtVec.push_back( &LatchButt1 );

    inFile.open("images/on_offx2SSdata.txt");
    if( !inFile ) return false;
    onOffButtSS.init( inFile );
    inFile.close();
    onOffButtSS.copySet( Rinit, 0 );// 3 sets
    label.setString("");
    onOffButt.init( Rinit, 100.0f, 400.0f, label, 0.4f );
    onOffButt.pHitFunc = [this](){ std::cout << "\nOOBUTT: " << ( onOffButt.sel ? "on" : "off" ); };
    button::RegisteredButtVec.push_back( &onOffButt );

    // load a spriteSheet for slip buttons
    inFile.open("images/sixSlipButtsSSdata.txt");
    if( !inFile ) return false;
    SSbuttSlip.init( inFile );
    size_t sn = 0; inFile >> sn;// read set number
    inFile.close();
    sf::RectangleShape Rsb[6];
    SSbuttSlip.copySet( Rsb, sn );
 //   imgRect = Rsb[1];
    label.setString("");
    slipButt1.init( Rsb, 100.0f, 300.0f, label, 0.6f );
    slipButt1.pHitFunc = [this](){ std::cout << "\nSB1: " << ( slipButt1.sel ? "yes" : "no" ); };
  //  slipButt1.pHitFunc = [this](){ std::cout << "\nhit!"; };
    button::RegisteredButtVec.push_back( &slipButt1 );

    // textButtons
    label.setString("message");
    msgButt.cursorToRight = true;
    msgButt.maxCharCount = 5;
    msgButt.init( 500.0f, 10.0f, 70.0f, 26.0f, label );
    msgButt.pStrFunc = [this](const char* str) { buttMsg.setString( str ); };
    button::RegisteredButtVec.push_back( &msgButt );
    buttMsg.setFont( *button::pFont );
    buttMsg.setCharacterSize(20);
    buttMsg.setColor( sf::Color::Red );
    buttMsg.setPosition( 500.0f, 70.0f );

    init_textList( txtButtList, 800.0f, 10.0f );
    button::RegisteredButtVec.push_back( &txtButtList );

    sf::Vector2f hbPos( 700.0f, 150.0f ), csPos( 700.0f, 200.0f );
    motionControl.init( hbPos, csPos, sf::Vector2f(400.0f, 200.0f), "Motion" );
    button::RegisteredButtVec.push_back( &motionControl );

    accDial.pDialTxt = &dialTxt;
    accDial.pNeedleTxt = &needleTxt;
    label.setString("Accel"); accDial.label = label; accDial.label.setColor( sf::Color::Red );
    sf::Vector2f dialPos( csPos.x + 20.0f, csPos.y + 20.0f ), dialSep( 120.0f, 0.0f );
    if( !accDial.init( dialPos, "include/levels/buttons/dialDisplay_init.txt" ) ) { std::cout << "\nno dial init"; return false; }
    posDial = velDial = accDial;
    brakeRate = accDial.maxVal;
    velDial.minVal = -120.0f; velDial.maxVal = 120.0f; velDial.label.setString("Velocity"); velDial.setPosition( dialPos + dialSep );
    posDial.minVal = -50000.0f; posDial.maxVal = 50000.0f; posDial.label.setString("Position"); posDial.setPosition( dialPos + 2.0f*dialSep );
    label.setString("accel");
    dialSlide.init( dialPos.x + 80.0f, dialPos.y + 140.0f, 200.0f, 24.0f, label, accDial.minVal, accDial.maxVal, accDial.currVal, [this](float x){ accDial.setValue(x); atHome = false; } );
 //   button::RegisteredButtVec.push_back( &dialSlide );
    motionControl.pButtVec.push_back( &dialSlide );

    fudgeBS.init( sf::Vector2f(dialPos.x + 80.0f, dialPos.y + 90.0f), sf::Vector2f(50.0f,0.0f), 160.0f, 26.0f, "fudge", "fudge", 0.85f, 1.0f, 0.9967f, nullptr );
    motionControl.pButtVec.push_back( &fudgeBS );

    label.setString("Brake");
    brakeButt.init( dialPos.x, dialPos.y + 80.0f, 50.0f, 36.0f, [this](){ dialSlide.set_fVal(0.0f); }, label );// Brake
    brakeButt.mode = 'I';
 //   button::RegisteredButtVec.push_back( &brakeButt );
    motionControl.pButtVec.push_back( &brakeButt );
    label.setString("Home");
    homeButt.init( dialPos.x, dialPos.y + 130.0f, 40.0f, 30.0f, [this](){ float acc = velDial.currVal*velDial.currVal/(2.0f*posDial.currVal); if( fudgeBS.sel ) acc *= fudgeBS.voh.fVal; dialSlide.set_fVal( acc ); }, label );// Stop at home
 //   button::RegisteredButtVec.push_back( &homeButt );
    motionControl.pButtVec.push_back( &homeButt );
    motionControl.drawAll = [this](sf::RenderTarget& RT)
    {
        accDial.draw(RT);
        velDial.draw(RT);
        posDial.draw(RT);
    };

    moveCirc.setRadius( 14.0f );
    moveCirc.setOrigin( 14.0f, 14.0f );
    moveCirc.setFillColor( sf::Color::Magenta );
    moveCirc.setPosition( Level::winW/2.0f*( 1.0f + posDial.currVal/posDial.maxVal ), 500.0f );
    stayCirc.setRadius( 4.0f );
    stayCirc.setOrigin( 4.0f, 4.0f );
    stayCirc.setFillColor( sf::Color::Yellow );
    stayCirc.setPosition( Level::winW/2.0f, 480.0f );
    stopPtCirc.setRadius( 4.0f );
    stopPtCirc.setOrigin( 4.0f, 4.0f );
    stopPtCirc.setFillColor( sf::Color::Red );
    stopPtCirc.setPosition( Level::winW/2.0f, 480.0f );

    stopDistMsg.setFont( *button::pFont );
    stopDistMsg.setCharacterSize(20);
    stopDistMsg.setColor( sf::Color::Red );
    stopDistMsg.setPosition( Level::winW/2.0f, 520.0f );
    to_SF_string( stopDistMsg, posDial.currVal );
    atHome = true;
    stopDistPxMsg = stopDistMsg;
    stopDistPxMsg.setColor( sf::Color::Green );
    stopDistPxMsg.setPosition( Level::winW/2.0f, 550.0f );
    to_SF_string( stopDistPxMsg, posDial.currVal*Level::winW/(2.0f*posDial.maxVal) );

    return true;
}

bool lvlButts::handleEvent( sf::Event& rEvent )
{
    (void)rEvent;
    return true;
}

void lvlButts::update( float dt )
{
    static bool bbSelLast = false;

    if( brakeButt.sel )
    {
        if( abs( velDial.currVal ) <= brakeRate ) { velDial.currVal = 0.0f; accDial.setValue( 0.0f ); }
        else if( velDial.currVal < 0.0f ) accDial.setValue( brakeRate );
        else accDial.setValue( -brakeRate );
    }
    else if( bbSelLast ) accDial.setValue( 0.0f );

    float newVel = velDial.currVal + accDial.currVal*dt;

    static int updateDelay = 0;
    const int updateFreq = 10;// update accel value every 10 frames
    ++updateDelay %= updateFreq;
    // stop at home - direction changed, so stop
    if( homeButt.sel )
    {

        //               delay                       not past home yet                    watch denominator ahead
        //if( ++updateDelay%updateFreq == 0 && posDial.currVal*velDial.currVal < 0.0f && abs( posDial.currVal ) > 1.0f )
        if( !fudgeBS.sel && updateDelay == 0 && posDial.currVal*velDial.currVal < 0.0f && abs( posDial.currVal ) > 1.0f )
            { dialSlide.set_fVal( velDial.currVal*velDial.currVal/(2.0f*posDial.currVal) ); }
        // at stop point
        if( newVel*velDial.currVal <= 0.0f )
        {
            homeButt.setSel(false);
            dialSlide.set_fVal( 0.0f );
            newVel = 0.0f;
            std::cout << "\nstop at: " << posDial.currVal;
            to_SF_string( stopDistMsg, posDial.currVal );
            to_SF_string( stopDistPxMsg, posDial.currVal*Level::winW/(2.0f*posDial.maxVal) );
            atHome = true;
            updateDelay = 0;
        }
    }
    else if( motionControl.sel && updateDelay == 0 && posDial.currVal*velDial.currVal < 0.0f )
    {
        float stopDist = velDial.currVal*velDial.currVal/(2.0f*accDial.maxVal);// assumed v < 0 so approaching from right
        if( velDial.currVal > 0.0f ) stopDist *= -1.0f;// approaching from left
        stopPtCirc.setPosition( Level::winW/2.0f*( 1.0f + stopDist/posDial.maxVal ) , 480.0f );
    }

    // stop over travel
    if( newVel > velDial.maxVal || newVel < velDial.minVal ) dialSlide.set_fVal( 0.0f );// prevent overspeed
    velDial.setValue( newVel );
    float newPos = posDial.currVal + velDial.currVal*dt;
    if( newPos > posDial.maxVal || newPos < posDial.minVal ) { dialSlide.set_fVal( 0.0f ); velDial.setValue( 0.0f ); }// prevent leaving area
    posDial.setValue( posDial.currVal + velDial.currVal*dt );

    moveCirc.setPosition( Level::winW/2.0f*( 1.0f + posDial.currVal/posDial.maxVal ), 500.0f );

    bbSelLast = brakeButt.sel;
    return;
}

void lvlButts::draw( sf::RenderTarget& RT ) const
{
    RT.draw( buttMsg );
    if( motionControl.sel && posDial.currVal*velDial.currVal < 0.0f )RT.draw( stopPtCirc );
    RT.draw( stayCirc );
    RT.draw( moveCirc );
    if( atHome ){ RT.draw( stopDistMsg ); RT.draw( stopDistPxMsg ); }
    return;
}

void lvlButts::cleanup()
{
    return;
}

// non members
void init_textList( buttonList& textList, float posx, float posy )
{
    sf::Text txt( "text", *button::pFont, 10);
    txt.setColor( sf::Color(0,0,0) );
    float wButt = 70.0f, hButt = 25.0f;
    textList.init( posx, posy, wButt, hButt, txt );

    posy += hButt;
    hButt = 30.0f; wButt = 150.0f;
    textButton* pTB = new textButton( posx, posy, wButt, hButt, "text button 1", 15 );
    pTB->pStrFunc = [](const char* str) { std::cout << "\n#1 " << str; };
    textList.pButtVec.push_back( pTB );
    pTB = new textButton( posx, posy+=hButt + 10.0f, wButt, hButt, "text button 2", 15, true );
    pTB->pStrFunc = [](const char* str) { std::cout << "\n#2 " << str; };
    textList.pButtVec.push_back( pTB );
    pTB = new textButton( posx, posy+=hButt + 10.0f, wButt, hButt, "text button 3", 8 );
    pTB->pStrFunc = [](const char* str) { std::cout  << "\n#3 " << str; };
    textList.pButtVec.push_back( pTB );

    textList.persist = 3;
    textList.ownButts = true;
}
