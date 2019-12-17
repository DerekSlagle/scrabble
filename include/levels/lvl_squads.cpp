#include "lvl_squads.h"

bool squad::init( std::istream& is, std::vector<Leg*>& PathVec )
{
    unsigned int numShips, pathIdx;
    int clr = 0;
    is >> pathIdx >> numShips >> clr;
    if( !is.good() ) { anyAlive = false; std::cerr << "\nno more squad data"; return false; }
    std::cerr << "\nnumShips = " << numShips;
    is >> startDelay >> dtLaunch >> v0;
    std::cout << "\nstartDelay: " << startDelay;
    float cs, cd, radius, M;
    is >> cs >> cd >> radius >> M;
    is >> iHealth;
    if( dtLaunch > 0.0f ) is >> offX >> offY;
 //   int clr = 'R';// is >> clr;
    sf::Color fillColor = sf::Color(100,100,100);//clr == 0 ? sf::Color::Red : ( clr == 1 ? sf::Color::Green : sf::Color::Blue );
    switch( clr )
    {
        case 0: fillColor = sf::Color::Red; break;
        case 1: fillColor = sf::Color::Green; break;
        case 2: fillColor = sf::Color::Blue; break;
        case 3: fillColor = sf::Color::Cyan; break;
        case 4: fillColor = sf::Color::Magenta; break;
        case 5: fillColor = sf::Color::Yellow; break;
    }

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
        if( dtLaunch == 0.0f ) is >> offX >> offY;
        ITvec[i].init( pLeg, [this,i](float x,float y){ circVec[i].setPosition(x,y); }, v0, offX, offY );
        ITvec[i].reset( v0, i*dtLaunch );
        ITvec[i].initInertial( cs, cd, radius, M );
        ITvec[i].health = iHealth;
        ITvec[i].pSetHealth = [this,i](float x){ to_SF_string( healthMsgVec[i], x); };
        ITvec[i].pSetHealth( iHealth );
        ITvec[i].v1x = ITvec[i].v1y = ITvec[i].vLastX = ITvec[i].vLastY = 0.0f;
    }

    anyAlive = true;

    return is.good();
}

void squad::reStart()// same data as last
{
    unsigned int numShips = ITvec.size();
    if( circVec.size() != numShips || healthMsgVec.size() != numShips ) return;// broken

    for( unsigned int i = 0; i < numShips; ++i )
    {
     //   circVec[i].setRadius( radius );
     //   circVec[i].setOrigin( radius, radius );
     //   circVec[i].setFillColor( fillColor );
     //   healthMsgVec[i].setFont( *button::pFont );
     //   healthMsgVec[i].setColor( sf::Color::White );
     //   healthMsgVec[i].setCharacterSize( 20 );
     //   ITvec[i].init( pLeg, [this,i](float x,float y){ circVec[i].setPosition(x,y); }, v0, offX, offY );
        ITvec[i].reset( v0, i*dtLaunch );
     //   ITvec[i].initInertial( cs, cd, radius, M );
        ITvec[i].health = iHealth;
     //   ITvec[i].pSetHealth = [this,i](float x){ to_SF_string( healthMsgVec[i], x); };
        ITvec[i].pSetHealth( iHealth );
        ITvec[i].v1x = ITvec[i].v1y = ITvec[i].vLastX = ITvec[i].vLastY = 0.0f;
    }

    anyAlive = true;
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

    // load 1st squad
    squadVec.resize( maxSquads );
    for( auto& sq : squadVec ) sq.anyAlive = false;
    squadIn.open("include/levels/squads/squadList.txt");
    if( !squadIn ) { std::cout << "\nno squadList"; return false; }
    if( squadIn ) loadSquad();

    // a bomb
    fin.open("include/levels/squads/bomb_data.txt");
    if( !fin ) { std::cout << "\nno bomb_data"; return false; }
    float Fmax, v_Wave, R_full, R_max, R_eff;
    fin >> Fmax >> v_Wave >> R_full >> R_max >> R_eff;
    theBomb.init( Fmax, v_Wave, R_full, R_max, R_eff );

    shotBombVec.resize( maxBombShots );
    for( auto& sb : shotBombVec ) sb.myBomb = theBomb;
    for( auto& sh : shotBombVec ) sh.init( 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 8.0f, -1.0f, sf::Color::Magenta );
    for( unsigned int i = 0; i < 4; ++i ) xHair[i].color = sf::Color::Red;

    // theLaser
    theLaser.init( 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, sf::Color::Red );// 5th = damage

    sf::Text label("Laser", *button::pFont, 12);
    label.setColor( sf::Color::Black );
 /*   laserDamageStrip.init( 10.0f, 20.0f, 160.0f, 20.0f, label, 0.1f, 20.0f, theLaser.damage, &theLaser.damage );
    button::RegisteredButtVec.push_back( &laserDamageStrip );
    label.setString("acc_t");
    laserAcc_tStrip.init( 10.0f, 70.0f, 160.0f, 20.0f, label, -10.0f, 10.0f, 0.0f, &theLaser.acc_t );
    button::RegisteredButtVec.push_back( &laserAcc_tStrip );
    label.setString("Lock");
    beamLockButt.init( 10.0f, 100.0f, 40.0f, 20.0f, [this](){ theLaser.beamLock = beamLockButt.sel; }, label );
    button::RegisteredButtVec.push_back( &beamLockButt );
    label.setString("force");
    fceSelect.init( 60.0f, 100.0f, 20.0f, 80.0f, 3, label, 2 );// radioButton
    button::RegisteredButtVec.push_back( &fceSelect );
    if( fceSelect.buttVec.size() < 3 ) { std::cout << "\nRB problem"; return false; }
    std::vector<std::string> strVec; strVec.push_back("push"); strVec.push_back("pull"); strVec.push_back("osc");
  //  std::cout << "\n**** ";
    fceSelect.setButtLabels( 20.0f, 0.0f, strVec, 12, sf::Color::White );
    fceSelect.pFuncIdx = [this]( size_t i )
    {
        switch(i)
        {
            case 0: theLaser.push = true;  theLaser.doOscillate = false; break;
            case 1: theLaser.push = false; theLaser.doOscillate = false; break;
            case 2: theLaser.push = true;  theLaser.doOscillate = true; break;
        }
    };  */

    squadIter = squadVec.begin();

    makeBrakeControl( sf::Vector2f(260.0f, 10.0f), sf::Vector2f(320.0f, 10.0f) );
    button::RegisteredButtVec.push_back( &brakeControl );

    label.setString("shots");
    sf::Vector2f sclPos( 520.0f, 10.0f );
    shotControlList.init( sclPos.x, sclPos.y, 50.0f, 30.0f, label );
    shotControlList.pButtVec.push_back( &shotControl );
    shotControlList.pButtVec.push_back( &bombControl );
    shotControlList.pButtVec.push_back( &clusterControl );
    shotControlList.pButtVec.push_back( &gdShotControl );
    shotControlList.pButtVec.push_back( &laserControl );
    shotControlList.persist = 3;
    button::RegisteredButtVec.push_back( &shotControlList );

    sf::Vector2f csPos = sclPos; csPos.x += 60.0f; sclPos.y += 27.0f;
    makeShotControl( sclPos, csPos ); sclPos.y += 27.0f;
    makeBombControl( sclPos, csPos ); sclPos.y += 27.0f;
    clShot.init( 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 8.0f, -1.0f, sf::Color::Magenta );
    clShot.numFrags = 7;
    clShot.vFrags = 30.0f;
    clShot.tLifeFrags = 50.0f;
    clShot.pGetShot = [this]() { return getShot(); };
    makeClusterControl( sclPos, csPos );  sclPos.y += 27.0f;
    makeGuideShotControl( sclPos, csPos ); sclPos.y += 27.0f;
    gdShot.init( 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 7.0f, -1.0f, sf::Color::White );
    makeLaserControl( sclPos, csPos );

    // fling ball
    flingBall.setRadius(20.0f);
    flingBall.setOrigin(20.0f,20.0f);
    if( !ballTxtA.loadFromFile("images/soccerBallC.png") ) { std::cout << "no ballTxtA image data\n"; return false; }
    flingBall.setTexture( &ballTxtA );
    flingIA.init( nullptr, [this](float x,float y){ flingBall.setPosition(x,y); }, 0.0f );
 //   flingIA.reset( 0.0f );
    flingIA.initInertial( 0.1f, 0.1f, flingBall.getRadius(), flingMass );
    flingIA.setPosition( 50.0f, 200.0f );
    flingIA.pLeg0 = &flingLeg;

    float shotRad = 5.0f;
    shotVec.resize( maxShots );
    for( auto& sh : shotVec ) sh.init( 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, shotRad, -1.0f, sf::Color::Red );

    return true;
}

void lvl_squads::loadSquad()
{
    for( auto& sq : squadVec )
    {
        if( !sq.anyAlive )// available
        {
            sq.init( squadIn, pathVec );
            loadTime = sq.startDelay;
            if( !squadIn.good() ) squadIn.close();
            return;
        }
    }
}

bool lvl_squads::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::B )// bomb
        {
            theBomb.detonate( button::mseX, button::mseY );
        }
        else if ( rEvent.key.code == sf::Keyboard::Space )// shoot from mouseLeg
        {
            if( button::mseDnLt ){ if( !doFire ) fireTime = fireDelay; doFire = true; }// 1st shot right away
        }
        else if ( rEvent.key.code == sf::Keyboard::F3 )// ShotBomb
        {
            if( button::mseDnLt ){ if( !doFireSB ) fireTime = fireDelay; doFireSB = true; }// 1st shot right away
        }
        else if ( rEvent.key.code == sf::Keyboard::F4 )// clusterShot
        {
            if( !clShot.inUse() ) { fireTime = fireDelay; doFireCS = true; }
        }
        else if ( rEvent.key.code == sf::Keyboard::F5 )// guidedShot
        {
            if( !gdShot.inUse() )
            {
                fireTime = fireDelay; doFireGD = true;
                if( !squadIter->ITvec.empty() && squadIter->anyAlive )
                for( auto& IT : squadIter->ITvec )
                    if( IT.health > 0.0f )
                    {
                        gdShot.pTgtX = &IT.x1; gdShot.pTgtY = &IT.y1;
                        gdShot.pTgtVx = &IT.v1x; gdShot.pTgtVy = &IT.v1y;
                   //     std::cout << "\nTarget assigned";
                   //     IT.v1x += 3.0f;
                        break;
                    }
            }
        }
        else if ( rEvent.key.code == sf::Keyboard::L )// theLaser
        {
            theLaser.state = true;
        }

    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::Space ) doFire = false;
        else if ( rEvent.key.code == sf::Keyboard::F3 ) doFireSB = false;
        else if ( rEvent.key.code == sf::Keyboard::L ) theLaser.state = false;// theLaser
    }

    return true;
}

Shot* lvl_squads::getShot()
{
    for( auto& fm : shotVec )
        if( !fm.inUse() ) return &fm;

    return nullptr;
}

ShotBomb* lvl_squads::getShotBomb()
{
    for( auto& fm : shotBombVec )
        if( !fm.inUse() ) return &fm;

    return nullptr;
}

void lvl_squads::update( float dt )
{
    inertialAdapter* pIA = &flingIA;

    if( button:: pButtMse ) flingIA.update( dt );//, 0.0f, gravity.y );
    else// mse not over a button
    {
        if( button::clickEvent_Lt() == 1 )
        {
            if( flingIsFree )
            {
                flingIA.setPosition( button::mseX, button::mseY );
                flingIA.v1x = flingIA.v1y = 0.0f;
            }
            fireGrabPos = button::msePos();
        }
        else if( button::clickEvent_Lt() == -1 )// release
        {
            if( flingIsFree )
            {
                sf::Vector2f sep( button::mseX - flingIA.x1, button::mseY - flingIA.y1 );
                flingIA.v1x = 0.1f*sep.x; // 10 px/fr at 100 px distance
                flingIA.v1y = 0.1f*sep.y;
            }
        }
        else if( button::clickEvent_Rt() == 1 )// capture to flingLeg
        {
            flingIsFree = !flingIsFree;// toggle
            if( flingIsFree )
            {
                pIA->pLeg = nullptr;
            }
            else
            {
                pIA->reset(0.0f);
            }

        }
//        else if( !button::mseDnLt ) flingIA.update( dt );//, 0.0f, gravity.y );
        else if( !flingIsFree || !button::mseDnLt ) flingIA.update( dt );//, 0.0f, gravity.y );// update if on mouseLeg or free but not held
    }

    if( flingIsFree ) flingIA.keepInBox( 0.0f, Level::winW, 0.0f, Level::winH );

    // firing
    if( !button::pButtMse && button::mseDnLt )
    {
        sf::Vector2f vel;
        if( flingIsFree ){ vel.x = button::mseX - pIA->x1; vel.y = button::mseY - pIA->y1; }
        else{ vel.x = pIA->x1 - fireGrabPos.x; vel.y = pIA->y1 - fireGrabPos.y; }// legbound
        float vMag = sqrtf( vel.x*vel.x + vel.y*vel.y );// normalize
        if( vMag > 10.0f )
        {
            if( button::didScroll )// adjust shotSpeed
                shotMS.setValue( "tLife", shotLife + button::scrollAmount );

            vel *= shotSpeed/vMag;
            sf::Vector2f posF( pIA->x1 + vel.x*shotLife,  pIA->y1 + vel.y*shotLife );
      //      posF -= vel;
            const float xhW = 10.0f;
            for( unsigned int i = 0; i < 4; ++i ) xHair[i].position = posF;
            xHair[0].position.x -= xhW; xHair[1].position.x += xhW;
            xHair[2].position.y -= xhW; xHair[3].position.y += xhW;
            if( ( doFire || doFireSB || doFireCS || doFireGD ) && (fireTime += dt) > fireDelay )
            {
                fireTime = 0.0f;
                if( doFireCS )
                {
                    clShot.reset( pIA->x1, pIA->y1, vel.x, vel.y, shotLife );
                    doFireCS = false;
                }
                else if( doFireGD )
                {
                    vel.x *= gdVelStrip.fVal/shotSpeed;
                    vel.y *= gdVelStrip.fVal/shotSpeed;
                    gdShot.reset( pIA->x1, pIA->y1, vel.x, vel.y, shotLife );
                    doFireGD = false;
                }
                else
                {
                    Shot* pShot = nullptr;
                    if( doFire ) pShot = getShot();
                    else pShot = getShotBomb();
                    // shot is available
                    if( pShot ){ pShot->reset( pIA->x1, pIA->y1, vel.x, vel.y, shotLife ); pShot->circ.setFillColor( sf::Color::Red ); }
                }
            }

            // laser

            if( theLaser.state )            {
                theLaser.beam[0].color = theLaser.beam[1].color = sf::Color::Red;
                theLaser.beamLength = sqrtf( vel.x*vel.x + vel.y*vel.y )*shotLife;
                theLaser.update( pIA->x1, pIA->y1, vel.x, vel.y );
                for( auto& sqd : squadVec )
                {
                    if( !sqd.anyAlive ) continue;
                    for( auto& it : sqd.ITvec ) if( it.health > 0.0f ) theLaser.hitFreeLine( it.x1, it.y1, it.v1x, it.v1y, it.R, it.m );
                }
            }
        }
        else
            for( unsigned int i = 0; i < 4; ++i ) { xHair[i].position.x = pIA->x1; xHair[i].position.y = pIA->y1; }
    }

    // hit testing
    for( auto& sqd : squadVec )// vs flingBall
    {
        if( !sqd.anyAlive ) continue;
        sqd.update(dt);
        for( auto& it : sqd.ITvec )
            if( it.health > 0.0f &&  it.hitFree( pIA->x1, pIA->y1, pIA->v1x, pIA->v1y, pIA->R, pIA->m ) ) if( pIA->pSetPosition ) pIA->pSetPosition(pIA->x1,pIA->y1);
        //    if( (it.health > 0.0f ) && pIA->hitFreeLine( it.x1, it.y1, it.v1x, it.v1y, it.R, it.m ) ) if( it.pSetPosition ) it.pSetPosition(it.x1,it. y1);;
    }

    // shots
    for( auto& fm : shotVec )
    {
        fm.update(dt);
        if( fm.tLife > 0.0f )
        for( auto& sqd : squadVec )
        {
            if( !sqd.anyAlive ) continue;
            for( auto& it : sqd.ITvec )
                if( it.health > 0.0f ) fm.hitFreeLine( it.x1, it.y1, it.v1x, it.v1y, it.R, it.m );
        }
    }

    // shotBombs
    for( auto& fm : shotBombVec )
    {
        fm.update(dt);
        if( fm.inUse() )
        for( auto& sqd : squadVec )
        {
            if( !sqd.anyAlive ) continue;
            float Fx = 0.0f, Fy = 0.0f;
            for( auto& it : sqd.ITvec )
                if( it.health > 0.0f )
                {
                    if( fm.myBomb.isExploding() ) { if( fm.myBomb.getForce( it.x1, it.y1, Fx, Fy ) ) { it.v1x += Fx*dt/it.m; it.v1y += Fy*dt/it.m; } }
                    else fm.hitFreeLine( it.x1, it.y1, it.v1x, it.v1y, it.R, it.m );
                }
        }
    }

    theBomb.update(dt);
    if( theBomb.isExploding() )
    {
        float Fx = 0.0f, Fy = 0.0f;
        for( auto& sqd : squadVec )
        {
            if( !sqd.anyAlive ) continue;
            for( auto& it : sqd.ITvec )
                if( it.health > 0.0f && theBomb.getForce( it.x1, it.y1, Fx, Fy ) ) { it.v1x += Fx*dt/it.m; it.v1y += Fy*dt/it.m; }
        }
        // flingBall vs bomb
        if( theBomb.getForce( pIA->x1, pIA->y1, Fx, Fy ) ) { pIA->v1x += Fx*dt/pIA->m; pIA->v1y += Fy*dt/pIA->m; }
    }

    // clusterShot
    clShot.update(dt);
    if( clShot.inUse() )
    {
        for( auto& sqd : squadVec )
        {
            if( !sqd.anyAlive ) continue;
            for( auto& it : sqd.ITvec )
                if( it.health > 0.0f ) clShot.hitFreeLine( it.x1, it.y1, it.v1x, it.v1y, it.R, it.m );
        }
    }

    // guidedShot
    gdShot.update(dt);
    if( gdShot.inUse() )
    {
        for( auto& sqd : squadVec )
        {
            if( !sqd.anyAlive ) continue;
            for( auto& it : sqd.ITvec )
                if( it.health > 0.0f ) gdShot.hitFreeLine( it.x1, it.y1, it.v1x, it.v1y, it.R, it.m );
        }
    }

    // contain free
    for( auto& sqd : squadVec )
    {
        if( !sqd.anyAlive ) continue;
        for( auto& it : sqd.ITvec )
            if( !it.pLeg && it.health > 0.0f ) it.keepInBox( 0.0f, Level::winW, 0.0f, Level::winH );
     //       if( !it.pLeg && it.health > 0.0f ) it.keepInBox_soft( 0.0f, Level::winW, 0.0f, Level::winH );
    }

    // load next squad
    if( squadIn )
    {
        loadTime -= dt;
        if( loadTime <= 0.0f ) loadSquad();
    }

    return;
}

void lvl_squads::draw( sf::RenderTarget& RT ) const
{
 //   RT.draw( wasupMsg );
    theBomb.draw(RT);
    for( const auto& shot : shotVec ) shot.draw(RT);
    for( const auto& sb : shotBombVec ) sb.draw(RT);
    clShot.draw(RT);
    gdShot.draw(RT);
    theLaser.draw(RT);
    for( const auto& sqd : squadVec ) sqd.draw(RT);
 //   for( unsigned int i = 0; i < maxShots; ++i ) if( fmVec[i].tLife > 0.0f ) RT.draw( fmCircVec[i] );
    RT.draw( flingBall );
    if( !button::pButtMse && button::mseDnLt ) RT.draw( xHair, 4, sf::Lines );
    return;
}

void lvl_squads::cleanup()
{
    for( auto& pPath : pathVec )
        if( pPath ) std::cout << "\ndeleted " << destroyPath( pPath ) << " legs from path";
    return;
}

void lvl_squads::makeBrakeControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    brakeControl.init( hbPos, csPos, sf::Vector2f(180.0f, 230.0f), "Motion" );
    for( unsigned int i = 0; i < 7; ++i ){ brakeControl.pButtVec.push_back( &pathButt[i] ); pathButt[i].mode = 'I'; }
    sf::Text label("+vel", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    pathButt[0].init( csPos.x + 10.0f, csPos.y + 10.0f, 40.0f, 25.0f, nullptr, label );// +vel
    label.setString("-vel");
    pathButt[1].init( csPos.x + 10.0f, csPos.y + 37.0f, 40.0f, 25.0f, nullptr, label );// -vel
    label.setString("Brake");
    pathButt[2].init( csPos.x + 52.0f, csPos.y + 10.0f, 70.0f, 52.0f, nullptr, label );// Brake
    std::function<void(void)> pComFunc = [this]()
    {
        pathCirc.setFillColor( squadIter->circVec.empty() ? sf::Color::White : squadIter->circVec[0].getFillColor() );
        if( !squadIter->ITvec.empty() )
        {
            cSpringStrip.set_fVal( squadIter->ITvec[0].Cs );
            dampStrip.set_fVal( squadIter->ITvec[0].Cd );
        }
    };
    label.setString("<--");
    pathButt[3].init( csPos.x + 10.0f, csPos.y + 80.0f, 30.0f, 24.0f, [this,pComFunc](){ if( squadIter == squadVec.begin() ) squadIter = squadVec.end()-1; else --squadIter; pComFunc(); }, label );// previous squad
    label.setString("-->");
    pathButt[4].init( csPos.x + 90.0f, csPos.y + 80.0f, 30.0f, 24.0f, [this,pComFunc](){ if( squadIter == squadVec.end()-1 ) squadIter = squadVec.begin(); else ++squadIter; pComFunc(); }, label );// next squad
    label.setString("reset");
    pathButt[5].init( csPos.x + 130.0f, csPos.y + 10.0f, 40.0f, 52.0f, [this](){ if( !squadIter->ITvec.empty() ) squadIter->reStart(); }, label );// next squad
    label.setString("kill");
    pathButt[6].init( csPos.x + 130.0f, csPos.y + 80.0f, 40.0f, 30.0f, [this](){ if( !squadIter->ITvec.empty() ) squadIter->anyAlive = false; }, label );// next squad
    pathCirc.setRadius(15.0f);
    pathCirc.setOrigin(15.0f,15.0);
    pathCirc.setPosition( csPos.x + 65.0f, csPos.y + 90.0f  );
    pathCirc.setFillColor( squadIter->circVec[0].getFillColor() );

    label.setString("spring K");
    cSpringStrip.init( csPos.x + 10.0f, csPos.y + 130.0f, 110.0f, 20.0f, label, 0.01f, 0.3, 0.1, [this](float x){ if( !squadIter->ITvec.empty() ) for( auto& IT : squadIter->ITvec ) IT.Cs = x; } );
    label.setString("damping");
    dampStrip.init( csPos.x + 10.0f, csPos.y + 186.0f, 110.0f, 20.0f, label, 0.01f, 0.3, 0.1, [this](float x){ if( !squadIter->ITvec.empty() ) for( auto& IT : squadIter->ITvec ) IT.Cd = x; } );
    brakeControl.pButtVec.push_back( &cSpringStrip );
    brakeControl.pButtVec.push_back( &dampStrip );

    brakeControl.drawAll = [this](sf::RenderTarget& RT) { RT.draw( pathCirc ); };
    brakeControl.updateAll = [this]()
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

    brakeControl.setPositionAll = [this](sf::Vector2f Pos)
    {
        sf::Vector2f dPos = pathCirc.getPosition() - brakeControl.pos;
        pathCirc.setPosition( Pos + dPos );
    };
}

void lvl_squads::makeShotControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    shotControl.init( hbPos, csPos, sf::Vector2f(240.0f,120.0f), "shots" );
    shotControl.pButtVec.push_back( &shotMS );

    std::ifstream fin( "include/levels/squads/shotMS.txt" );
    if( !fin ) { std::cout << "\n no shotMS data "; return; }
    sf::Vector2f ofst(10.0f,10.0f);
    shotMS.init( csPos + ofst, &shotMsStrip, fin );
    fin.close();

    if( shotMS.sdataVec.size() != 7 ) { std::cout << "\nbad shotMS data"; return; }
    stripData* pSD = &shotMS.sdataVec.front();
    shotMsStrip.reInit( *pSD );// Grad
    pSD->pSetFunc =     [this](float x){ impactTarget::FceToDmgGrad = x; };// Grad
    impactTarget::FceToDmgGrad = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ impactTarget::dVelMin = x; };// dVel
    impactTarget::dVelMin = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ for( auto& fm : shotVec ) fm.m = x; };// mass
    pSD->pSetFunc( pSD->xCurr );
 //   (++pSD)->pSetFunc = [this](float x){ for( auto& fm : shotVec ) fm.R = x; for( auto& circ : fmCircVec ){ circ.setRadius(x); circ.setOrigin(x,x); } };// radius
    (++pSD)->pSetFunc = [this](float x){ for( auto& fm : shotVec ) { fm.R = x; fm.circ.setRadius(x); fm.circ.setOrigin(x,x); } };// radius
    pSD->pSetFunc( pSD->xCurr );
    (++pSD)->pSetFunc = [this](float x){ shotSpeed = x; };// shot speed
    shotSpeed = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ fireDelay = x; };// fireDelay
    fireDelay = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ shotLife = x; };// tLife
    shotLife = pSD->xCurr;
}

void lvl_squads::makeBombControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    bombControl.init( hbPos, csPos, sf::Vector2f(260.0f,100.0f), "BOMB" );
    bombControl.pButtVec.push_back( &bombMS );

    std::ifstream fin( "include/levels/squads/bombMS.txt" );
    if( !fin ) { std::cout << "\n no bombMS data "; return; }
    sf::Vector2f ofst(10.0f,10.0f);
    bombMS.init( csPos + ofst, &bombStrip, fin );
    fin.close();

    if( bombMS.sdataVec.size() != 5 ) { std::cout << "\nbad bombMS data"; return; }
    stripData* pSD = &bombMS.sdataVec.front();
    bombStrip.reInit( *pSD );// Grad
    pSD->pSetFunc =     [this](float x){ theBomb.Fapp = x; };// Grad
    theBomb.Fapp = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ theBomb.vWave = x; };// dVel
    theBomb.vWave = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ theBomb.Rfull = x; };// mass
    theBomb.Rfull = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ theBomb.Rmax = x; theBomb.R0 = theBomb.Rmax + 1.0f; };// radius
    theBomb.Rmax =  pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ theBomb.Reff = x; };// shot speed
    theBomb.Reff = pSD->xCurr;
    theBomb.R0 = theBomb.Rmax + 1.0f;
}

void lvl_squads::makeClusterControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    clusterControl.init( hbPos, csPos, sf::Vector2f(230.0f,100.0f), "CLUST" );
    clusterControl.pButtVec.push_back( &clusterMS );

    sf::Text label("nFrags", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    clusterMsStrip.init( csPos.x+10.0f, csPos.y+46.0f, 160.0f, 20.0f, label, 3.0f, 20.0f, clShot.numFrags, [this](float x){ clShot.numFrags = floor(x); } );
    clusterMS.pFS = &clusterMsStrip;
    clusterMS.sel = true;

 //   label.setString("SB");
    std::function<Shot*(void)> pGetShot = [this](){ return getShot(); }, pGetShotBomb = [this](){ return getShotBomb(); };
    useSbButt.init( csPos.x+180.0f, csPos.y+10.0f, 20.0f, 20.0f, [this,pGetShot,pGetShotBomb](){ clShot.pGetShot = useSbButt.sel ? pGetShotBomb : pGetShot; }, "SB" );
    clusterControl.pButtVec.push_back( &useSbButt );

    float buttW = 40.0f, buttH = 20.0f, spcX = 10.0f;
    sf::Vector2f buttPos( csPos.x+10.0f, csPos.y+10.0f );
    clusterMS.sdataVec.push_back( stripData( "nFrags", 3, 20, clShot.numFrags, [this](float x){ clShot.numFrags = floor(x); } ) );// numFrags
    clusterMS.selButtVec.push_back( buttonRect( buttPos.x, buttPos.y, buttW, buttH, nullptr, "nFrags" ) );
    clusterMS.sdataVec.push_back( stripData( "vFrags", 10.0f, 70.0f, clShot.vFrags, [this](float x){ clShot.vFrags = x; } ) );// vFrags
    clusterMS.selButtVec.push_back( buttonRect( buttPos.x += buttW + spcX, buttPos.y, buttW, buttH, nullptr, "vFrags" ) );
    clusterMS.sdataVec.push_back( stripData( "fragLife", 5.0f, 100.0f, clShot.tLifeFrags, [this](float x){ clShot.tLifeFrags = x; } ) );// tLifeFrags
    clusterMS.selButtVec.push_back( buttonRect( buttPos.x += buttW + spcX, buttPos.y, buttW, buttH, nullptr, "tLife" ) );

    clusterMsStrip.reInit( clusterMS.sdataVec.front() );
    if( !clusterMS.selButtVec.empty() ){ clusterMS.idx_curr = 0; clusterMS.selButtVec[0].setSel(true); }
}

void lvl_squads::makeGuideShotControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    gdShotControl.init( hbPos, csPos, sf::Vector2f(220.0f,170.0f), "Homer" );

    sf::Text label("speed", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    gdVelStrip.init( csPos.x+10.0f, csPos.y+20.0f, 120.0f, 20.0f, label, 5.0f, 70.0f, 20.0f, nullptr );
    gdShotControl.pButtVec.push_back( &gdVelStrip );
    label.setString("acc T");
    gdAtStrip.init( csPos.x+10.0f, csPos.y+74.0f, 120.0f, 20.0f, label, 0.0f, 1.0f, 0.0f, [this](float x){ gdShot.a_t = x; } );
    label.setString("acc P");
    gdApStrip.init( csPos.x+10.0f, csPos.y+128.0f, 120.0f, 20.0f, label, 0.01f, 1.0f, 0.2f, [this](float x){ gdShot.a_p = x; } );
    gdShotControl.pButtVec.push_back( &gdAtStrip );
    gdShotControl.pButtVec.push_back( &gdApStrip );
}

void lvl_squads::makeLaserControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    laserControl.init( hbPos, csPos, sf::Vector2f(240.0f,170.0f), "Laser" );

    sf::Text label("acc_p", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    laserDamageStrip.init( csPos.x + 10.0f, csPos.y + 20.0f, 160.0f, 20.0f, label, 0.1f, 20.0f, theLaser.damage, &theLaser.damage );
    laserControl.pButtVec.push_back( &laserDamageStrip );
    label.setString("acc_t");
    laserAcc_tStrip.init( csPos.x + 10.0f, csPos.y + 70.0f, 160.0f, 20.0f, label, -10.0f, 10.0f, 0.0f, &theLaser.acc_t );
    laserControl.pButtVec.push_back( &laserAcc_tStrip );
    label.setString("Lock");
    beamLockButt.init( csPos.x + 10.0f, csPos.y + 120.0f, 40.0f, 20.0f, [this](){ theLaser.beamLock = beamLockButt.sel; }, label );
    laserControl.pButtVec.push_back( &beamLockButt );
    label.setString("force");
    fceSelect.init( csPos.x + 80.0f, csPos.y + 120.0f, 120.0f, 20.0f, 3, label, 2 );// radioButton
    laserControl.pButtVec.push_back( &fceSelect );
    if( fceSelect.buttVec.size() < 3 ) { std::cout << "\nRB problem"; return; }
    std::vector<std::string> strVec; strVec.push_back("push"); strVec.push_back("pull"); strVec.push_back("osc");
  //  std::cout << "\n**** ";
    fceSelect.setButtLabels( -10.0f, 12.0f, strVec, 12, sf::Color::White );
    fceSelect.pFuncIdx = [this]( size_t i )
    {
        switch(i)
        {
            case 0: theLaser.push = true;  theLaser.doOscillate = false; break;
            case 1: theLaser.push = false; theLaser.doOscillate = false; break;
            case 2: theLaser.push = true;  theLaser.doOscillate = true; break;
        }
    };
}
