#include "lvl_squads.h"

bool squad::init( std::istream& is, std::vector<Leg*>& PathVec )
{
    unsigned int numShips, pathIdx;
    int clr = 0;
    is >> pathIdx >> numShips >> clr;
    std::cerr << "\nnumShips = " << numShips;
    is >> startDelay >> dtLaunch >> v0;
    is >> offX >> offY;
    std::cout << "\nstartDelay: " << startDelay;
    float cs, cd, radius, M;
    is >> cs >> cd >> radius >> M;
    float health0; is >> health0;
 //   int clr = 'R';// is >> clr;
    sf::Color fillColor = clr == 0 ? sf::Color::Red : ( clr == 1 ? sf::Color::Green : sf::Color::Blue );

    if( pathIdx >= PathVec.size() ) return false;
    pLeg = PathVec[pathIdx];
    ITvec.resize( numShips );
    circVec.resize( numShips );
    healthMsgVec.resize( numShips );
    for( unsigned int i = 0; i < numShips; ++i )
    {
        circVec[i].setRadius( radius );
        circVec[i].setOrigin( radius, radius );
        circVec[i].setFillColor( fillColor );
        healthMsgVec[i].setFont( *button::pFont );
        healthMsgVec[i].setColor( sf::Color::White );
        healthMsgVec[i].setCharacterSize( 20 );
        ITvec[i].init( pLeg, [this,i](float x,float y){ circVec[i].setPosition(x,y); }, v0, offX, offY );
        ITvec[i].reset( v0, i*dtLaunch );
        ITvec[i].initInertial( cs, cd, radius, M );
        ITvec[i].health = health0;
        ITvec[i].pSetHealth = [this,i](float x){ to_SF_string( healthMsgVec[i], x); };
        ITvec[i].pSetHealth( health0 );
        ITvec[i].v1x = ITvec[i].v1y = ITvec[i].vLastX = ITvec[i].vLastY = 0.0f;
    }

    anyAlive = true;

    return is.good();
}

void squad::update( float dt )
{
    if( !anyAlive ) return;

    if( startDelay > 0.0f )
    {
        startDelay -= dt;
     //   std::cout << "\nstartDelay: " << startDelay;
        if( startDelay <= 0.0f ) std::cout << "\nsquadLaunched";
        return;
    }

//    std::cout << '*';
 //   for( auto& IT : ITvec ) IT.update(dt);
    anyAlive = false;
    for( unsigned int i = 0; i < ITvec.size(); ++i )
    {
        ITvec[i].update(dt);
        if( ITvec[i].health > 0.0f ){ healthMsgVec[i].setPosition( ITvec[i].x1, ITvec[i].y1 ); anyAlive = true; }
    }
}

void squad::draw( sf::RenderTarget& RT ) const
{
    if( startDelay > 0.0f ) return;
    if( !anyAlive ) return;

    for( unsigned int i = 0; i < ITvec.size(); ++i )
    {
        if( ITvec[i].health <= 0.0f ) continue;
        RT.draw( circVec[i] );
        RT.draw( healthMsgVec[i] );
    }
 //   for( const auto& crc : circVec ) RT.draw( crc );
 //   for( const auto& txt : healthMsgVec ) RT.draw( txt );
}

bool lvl_squads::init()
{
    Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,0,0);
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;

    sf::Text label("Wassup?", *button::pFont, 22);
    label.setColor( sf::Color::Red );
 //   wasupMsg = label;
 //   wasupMsg.setPosition( 600.0f,350.0f);

    // load some paths
    std::ifstream fin("include/levels/squads/pathList.txt");
    if( !fin ) { std::cout << "\nno pathList"; return false; }
    std::string pathFname;
    char pathType = 'N';// = Normal or 'F' = function type
    while( fin >> pathFname >> pathType )
    {
        pathVec.push_back(nullptr);
        std::ifstream pathFile( pathFname.c_str() );
        if( !fin ) { std::cout << "\nno " << pathFname; return false; }
        if( pathType == 'N' )
            makePath( pathFile, pathVec.back() );
        else if( pathType == 'F' )
            makeFuncPath( pathFile, pathVec.back() );
    }
    fin.close();

    // load some squads
    fin.open("include/levels/squads/squadList.txt");
    if( !fin ) { std::cout << "\nno squadList"; return false; }
    squad tmpSquad;
    unsigned int numSquads = 0; fin >> numSquads;
    squadVec.reserve( numSquads );
    do
    {
        squadVec.push_back( tmpSquad  );
    } while( squadVec.back().init( fin, pathVec ) );
    //   while( tmpSquad.init( fin, pathVec ) ) squadVec.push_back( tmpSquad  );


    squadIter = squadVec.begin();
    makePathControl( sf::Vector2f(200.0f, 10.0f), sf::Vector2f(260.0f, 10.0f) );
    button::RegisteredButtVec.push_back( &pathControl );

    // fling ball
    flingBall.setRadius(20.0f);
    flingBall.setOrigin(20.0f,20.0f);
    if( !ballTxtA.loadFromFile("images/soccerBallC.png") ) { std::cout << "no ballTxtA image data\n"; return false; }
    flingBall.setTexture( &ballTxtA );
    flingIA.init( nullptr, [this](float x,float y){ flingBall.setPosition(x,y); }, 0.0f );
 //   flingIA.reset( 0.0f );
    flingIA.initInertial( 0.1f, 0.1f, flingBall.getRadius(), flingMass );
    flingIA.setPosition( 50.0f, 200.0f );

    label.setString("fce grad");
    gradStrip.init( 20.0f, 20.0f, 120.0f, 20.0f, label, 0.01f, 0.4f, 0.1f, &impactTarget::FceToDmgGrad );
    button::RegisteredButtVec.push_back( &gradStrip );
    label.setString("dVelMin");
    dVelStrip.init( 20.0f, 80.0f, 120.0f, 20.0f, label, 0.3f, 10.0f, 8.0f, &impactTarget::dVelMin );
    button::RegisteredButtVec.push_back( &dVelStrip );

    return true;
}

bool lvl_squads::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
        if ( rEvent.key.code == sf::Keyboard::F3 ) pathControl.setPosition( sf::Vector2f(400.0f, 100.0f) );
    return true;
}

void lvl_squads::update( float dt )
{

    if( button:: pButtMse ) flingIA.update( dt );//, 0.0f, gravity.y );
    else// mse not over a button
    {
        if( button::clickEvent_Lt() == 1 )
        {
            flingIA.setPosition( button::mseX, button::mseY );
            flingIA.v1x = flingIA.v1y = 0.0f;
        }
        else if( button::clickEvent_Lt() == -1 )// release
        {
            sf::Vector2f sep( button::mseX - flingIA.x1, button::mseY - flingIA.y1 );
            flingIA.v1x = 0.1f*sep.x; // 10 px/fr at 100 px distance
            flingIA.v1y = 0.1f*sep.y;
        }
        else if( !button::mseDnLt ) flingIA.update( dt );//, 0.0f, gravity.y );
    }

    flingIA.keepInBox( 0.0f, Level::winW, 0.0f, Level::winH );

    inertialAdapter* pIA = &flingIA;
    for( auto& sqd : squadVec )
    {
        if( !sqd.anyAlive ) continue;
        sqd.update(dt);
        for( auto& it : sqd.ITvec )
            if( it.health > 0.0f &&  it.hitFree( pIA->x1, pIA->y1, pIA->v1x, pIA->v1y, pIA->R, pIA->m ) ) pIA->setPosition( pIA->x1, pIA->y1 );
    }

    return;
}

void lvl_squads::draw( sf::RenderTarget& RT ) const
{
 //   RT.draw( wasupMsg );
    for( const auto& sqd : squadVec ) sqd.draw(RT);
    RT.draw( flingBall );
    return;
}

void lvl_squads::cleanup()
{
    for( auto& pPath : pathVec )
        if( pPath ) std::cout << "\ndeleted " << destroyPath( pPath ) << " legs from path";
    return;
}

void lvl_squads::makePathControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{   pathControl.init( hbPos, csPos, sf::Vector2f(130.0f, 120.0f), "Motion" );
    for( unsigned int i = 0; i < 5; ++i ){ pathControl.pButtVec.push_back( &pathButt[i] ); pathButt[i].mode = 'I'; }
    sf::Text label("+vel", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    pathButt[0].init( csPos.x + 10.0f, csPos.y + 10.0f, 40.0f, 25.0f, nullptr, label );// +vel
    label.setString("-vel");
    pathButt[1].init( csPos.x + 10.0f, csPos.y + 37.0f, 40.0f, 25.0f, nullptr, label );// -vel
    label.setString("Brake");
    pathButt[2].init( csPos.x + 52.0f, csPos.y + 10.0f, 70.0f, 52.0f, nullptr, label );// Brake
    label.setString("<--");
    pathButt[3].init( csPos.x + 10.0f, csPos.y + 80.0f, 30.0f, 24.0f, [this](){ if( squadIter == squadVec.begin() ) squadIter = squadVec.end()-1; else --squadIter; pathCirc.setFillColor( squadIter->circVec[0].getFillColor() ); }, label );// previous squad
    label.setString("-->");
    pathButt[4].init( csPos.x + 90.0f, csPos.y + 80.0f, 30.0f, 24.0f, [this](){ if( squadIter == squadVec.end()-1 ) squadIter = squadVec.begin(); else ++squadIter; pathCirc.setFillColor( squadIter->circVec[0].getFillColor() ); }, label );// next squad
    pathCirc.setRadius(15.0f);
    pathCirc.setOrigin(15.0f,15.0);
    pathCirc.setPosition( csPos.x + 65.0f, csPos.y + 90.0f  );
    pathCirc.setFillColor( squadIter->circVec[0].getFillColor() );

    pathControl.drawAll = [this](sf::RenderTarget& RT) { RT.draw( pathCirc ); };
    pathControl.updateAll = [this]()
    {
        if( pathButt[0].sel ) for( auto& IT : squadIter->ITvec ) IT.v += accRate;// +vel
        else if( pathButt[1].sel ) for( auto& IT : squadIter->ITvec ) IT.v -= accRate;// -vel
        else if( pathButt[2].sel )
            for( auto& IT : squadIter->ITvec )// Brake
            {
                if( abs(IT.v) <= brakeRate )IT.v = 0.0f;
                else if( IT.v < 0.0f )IT.v += brakeRate;
                else IT.v -= brakeRate;
            }
    };

    pathControl.setPositionAll = [this](sf::Vector2f Pos)
    {
        sf::Vector2f dPos = pathCirc.getPosition() - pathControl.pos;
        pathCirc.setPosition( Pos + dPos );
    };
}
