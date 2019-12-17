#include "Level.h"

bool Level::run_update = true;
bool Level::step_update = true;
bool Level::do_capture = false;
Level* Level::pCurrLvl = nullptr;
Level* Level::pMM = nullptr;
sf::View Level::mapView;
float Level::mapW = 20.0f, Level::mapH = 20.0f;
float Level::viewOfstX = 0.0f, Level::viewOfstY = 0.0f;
bool Level::usingMapView = false;
sf::Color Level::clearColor(0,0,0);
buttonRect Level::goto_MMButt;
buttonRect Level::quitButt;
sf::Text Level::pauseMsg;

Level::Level()
{
    run_update = true;
    step_update = true;
    do_capture = false;
    Level::usingMapView = false;
    Level::clearColor = sf::Color::Black;
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;
    viewOfstX = 0.0f; viewOfstY = 0.0f;
    goto_MMButt.setSel(false);;
    goto_MMButt.setPosition( { winW/2.0f-goto_MMButt.sz.x,winH - 40.0f } );
    quitButt.setPosition( { winW/2.0f+quitButt.sz.x,winH - 40.0f } );
    button::RegisteredButtVec.clear();
}


Level::~Level()
{
    run_update = true;
    step_update = true;
    viewOfstX = 0.0f; viewOfstY = 0.0f;
    Level::usingMapView = false;

    Level::clearColor = sf::Color::Black;

    button::pButtMse = nullptr;// new
    button::pTxtButtFocus = nullptr;// new

    if( button::RegisteredButtVec.size() > 0 )
    {
        std::cout << "\n release " << button::RegisteredButtVec.size() << " controls";
        button::RegisteredButtVec.clear();
    }

}

void Level::init_stat( Level* p_MnMenu )
{
    Level::pCurrLvl = Level::pMM = p_MnMenu;

    if( button::pFont )
    {
        pauseMsg.setFont( *button::pFont );
        pauseMsg.setColor( sf::Color::Red );
        pauseMsg.setCharacterSize(30);
        pauseMsg.setString( "Pause" );
        pauseMsg.setPosition( 1050.0f, 20.0f );
        sf::Text label("Quit", *button::pFont, 12);
        label.setColor( sf::Color(0,0,255) );
        quitButt.init( (winW-60.0f)/2.0f, winH-35.0f, 60.0f, 28.0f, nullptr, label );
        label.setString("goto MM");
        goto_MMButt.init( (winW-160.0f)/2.0f, winH-35.0f, 60.0f, 28.0f, nullptr, label );
   //     goto_MMButt.init( (winW-goto_MMButt.sz.x)/2.0f, winH-40.0f, 60.0f, 28.0f, label );
        goto_MMButt.pHitFunc = [](){ Level::goto_MM(); };
    }
}

bool Level::run_stat( sf::RenderWindow& RW, float dt )
{
    button::reset();

    sf::Event event;
    while (RW.pollEvent(event))
        if( !Level::handleEvent_stat( event ) ) return false;

    Level::update_stat(dt);
    // draw
    RW.clear( clearColor );// sf::Color(0,64,128) )
    Level::draw_stat(RW);
    RW.display();

    return true;
}

bool Level::run_stat( sf::RenderWindow& RW, float dt, std::function<void(float)> pUpFunc, std::function<void(sf::RenderTarget& RT)> pDrawFunc )
{
    button::reset();

    sf::Event event;
    while (RW.pollEvent(event))
        if( !Level::handleEvent_stat( event ) ) return false;

    Level::update_stat(dt);
    if( pUpFunc ) pUpFunc(dt);
    // draw
    RW.clear( clearColor );// sf::Color(0,64,128) )
    Level::draw_stat(RW);
    if( pDrawFunc ) pDrawFunc(RW);
    RW.display();

    return true;
}

void Level::goto_MM()
{
    if( !pMM ) return;
    if( pCurrLvl && ( pCurrLvl != pMM ) ) delete pCurrLvl;
    pCurrLvl = pMM;
    pMM->reset();
}

bool Level::handleEvent_stat( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::Closed ) return false;

    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Escape ) return false;
        else if ( rEvent.key.code == sf::Keyboard::F1 ) { run_update = !run_update; }// toggle play/pause
        else if ( rEvent.key.code == sf::Keyboard::F2 ) { step_update = true; }// one step only if run_update==false
        else if ( rEvent.key.code == sf::Keyboard::M )
        {
            if( !button::pTxtButtFocus )
            {
                if( pCurrLvl != pMM ) goto_MM();
                return true;
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::C ) { do_capture = true; }
    }

    button::handleEvent( rEvent );

    if( !pCurrLvl ) return true;
    if( quitButt.sel ) return false;
    return pCurrLvl->handleEvent( rEvent );
}

void Level::update_stat( float dt )
{
    if( !pCurrLvl ) return;

    if( Level::usingMapView ) button::MseOverAll( viewOfstX, viewOfstY );
    else button::MseOverAll();

    if( run_update || step_update ) pCurrLvl->update(dt);
    step_update = false;
}

void Level::draw_stat( sf::RenderTarget& RT )
{
    if( !pCurrLvl ) return;

    if( usingMapView )
    {
         RT.setView( mapView );
         pCurrLvl->draw_map(RT);
         button::drawAll_map(RT);
         RT.setView( RT.getDefaultView() );
         pCurrLvl->draw(RT);
    }
    else
    {
        pCurrLvl->draw(RT);
    }

    button::drawAll_def(RT);
    if( !run_update ) RT.draw( pauseMsg );
}

// non members

bool hitRect( sf::Rect<float> hitRect, sf::Vector2f pt )
{
    if( pt.x < hitRect.left ) return false;
    if( pt.x > hitRect.left + hitRect.width ) return false;
    if( pt.y < hitRect.top ) return false;
    if( pt.y > hitRect.top + hitRect.height ) return false;

    return true;
}

void fitAinB( sf::FloatRect& A, sf::FloatRect B )// keep A within B
{
    if( A.left < B.left ) A.left = B.left;
    else if( A.left + A.width > B.left + B.width ) A.left = B.left + B.width -  A.width;
    if( A.top < B.top ) A.top = B.top;
    else if( A.top + A.height > B.top + B.height ) A.top = B.top + B.height -  A.height;
}
