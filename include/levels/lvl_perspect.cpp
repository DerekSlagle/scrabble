#include "lvl_perspect.h"

float myPI = 3.1415927f;
// lvl_perspect mfs
bool lvl_perspect::init()
{
    Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,0,0);
    button::setHoverBoxColor( Level::clearColor );

    // persPacks
    persPt::X0 = Level::winW/2.0f;
    std::ifstream finList("include/levels/perspect/persPackFileList.txt");
    if( !finList ) { std::cout << "\nno persPackFileList"; return false; }
    finList >> persPt::Yh >> persPt::Yg >> persPt::Z0;
    persPt::angPolar = 0.0f;
    persPt::Xv = persPt::Yv = persPt::Zv = 0.0f;
    persPack::viewPt.x = persPt::X0;
    persPack::viewPt.y = persPt::Yh;
    persPack::viewPt.z = 0.0f;//persPt::Z0;
    // load persMatrix
    std::string fileName;
    while( finList >> fileName )
    {
        fileName = "include/levels/perspect/" + fileName;
        std::ifstream f_in( fileName.c_str() );
        if( !f_in ) { std::cout << "\nno " << fileName; return false; }
        persMatrix pmTmp;
        if( !pmTmp.fromFile( f_in )  ) break;
        persMatVec.push_back( pmTmp );

        f_in.close();
    }
    finList.close();
    std::cout << "\n#persMatrix = " << persMatVec.size();

    if( !loadSpriteSheets() ) { std::cout << "\nloadSpriteSheets() fail"; return false; }
//    SSvec.back().img.createMaskFromColor( sf::Color::White );
//    SSvec.back().txt.loadFromImage( SSvec.back().img );

 //   if( !floorTxt.loadFromFile("images/trackLane1.jpg") ){ std::cout << "\nload floorTxt fail"; return false; }
    if( !floorTxt.loadFromFile("images/stoneTxt.png") ){ std::cout << "\nload floorTxt fail"; return false; }
    floorQuad.init( -2000.0f, -500.0f, 1000.0f, 1000.0f, 1250.0f, vec3f(0.0f,1.0f,0.0f), sf::Color::White );
    floorQuad.pTxt = &floorTxt;
    floorQuad.setTxtRect( sf::IntRect( 0, 0, floorTxt.getSize().x, floorTxt.getSize().y ), 'R', '1' );

    std::ifstream fin("include/levels/perspect/boxA_init.txt");
    if( !fin ) { std::cout << "\nno boxA_init"; return false; }
 //   if( !boxA.fromFile( fin, true ) ) { std::cout << "\nno boxA.fromFile fail"; return false; }
    if( !boxA.fromFile( fin, SSvec[3] ) ) { std::cout << "\nno boxA.fromFile fail"; return false; }
    fin.close();

    // more boxA stuff
    fin.open("include/levels/perspect/boxPath.txt");
    if( !fin ) { std::cout << "no boxPath data\n"; return false; }
    fin >> trackPos.x >> trackPos.y >> trackPos.z;
    fin >> trackDimX >> trackDimZ;
    makePath( fin, pLegTrack[0] );
    fin.close();
    fin.open("include/levels/perspect/boxPath_inLane.txt");
    if( !fin ) { std::cout << "no boxPath_inLane data\n"; return false; }
    makePath( fin, pLegTrack[1] );
    fin.close();
 //   LAboxA.init( pLegTrack[0], [this](float x, float y) { boxA.setPosition( x, boxA.pos.y, y ); }, 5.0f, 0.0f, 0.0f  );
 //   LAboxA.pLeg = nullptr;

    // more boxes = cars
    const unsigned int numCars = 12;
    carVec.reserve( numCars );// persBox
    carLAvec.reserve( numCars );// legAdapter
    float ds = 2.0f*pathLength( *pLegTrack[0] )/( numCars ), s0 = 0.0f, vCar = -5.0f;
    Leg *iter = pLegTrack[0], *pleg0 = pLegTrack[0];
    for( unsigned int i = 0; i < numCars; ++i )
    {
        if( i == numCars/2 )// 2nd half
        {
            ds = 2.0f*pathLength( *pLegTrack[1] )/( numCars ); s0 = 0.0f; vCar = 5.0f;
            iter = pLegTrack[1]; pleg0 = pLegTrack[1];
        }
        carVec.push_back( boxA );
        legAdapter tempLA;
        persBox& rCar = carVec.back();
        tempLA.init( pleg0, [this,&rCar,vCar](float x, float y) { rCar.setPosition( x, rCar.pos.y, y ); }, vCar, boxA.dimX/2.0f, boxA.dimY/2.0f );
        carLAvec.push_back( tempLA );
        legAdapter& rLA = carLAvec.back();
        while( iter && s0 > iter->len ) { s0 -= iter->len; iter = iter->next; }
        rCar.pos.y = -500.0f;
        rLA.s = s0;
        rLA.pLeg = iter;
        updateLAbox( rLA, rCar, 0.0f );
        s0 += ds;
    }

    flyIpos.x = -100.0f; flyIpos.y = 100.0f; flyIpos.z = persPt::z( persPt::Yh + dYh );
    flyFpos.x = -100.0f; flyFpos.y = 100.0f; flyFpos.z = persPt::Z0;
    flyBox.init( flyIpos.x, flyIpos.y, flyIpos.z, 100.0f, 80.0f, 100.0f, sf::Color::Yellow, false );// fly over scene
    flySpeed = 0.0f;
    vec3f dPos = flyFpos - flyIpos;
    flyBox.v = flySpeed*dPos/dPos.mag();

    track[0].init( trackPos );
    track[1].init( trackPos.x + trackDimX, trackPos.y, trackPos.z );
    track[2].init( trackPos.x + trackDimX, trackPos.y, trackPos.z + trackDimZ );
    track[3].init( trackPos.x, trackPos.y, trackPos.z + trackDimZ );
    for( unsigned int i = 0; i < 4; ++i ) trackQuad[i].position = track[i].screenPos();
    setTxtRect( trackQuad, SSvec[1].getFrRect(0,0) );
 //   setTxtRect( trackQuad, sf::IntRect( 0, 0, floorTxt.getSize().x, floorTxt.getSize().y ) );

    trackPQ.init( trackPos.x, trackPos.y, trackPos.z, trackDimX, trackDimZ, vec3f(0.0f,1.0f,0.0f), sf::Color::White );
    trackPQ.pTxt = &SSvec[1].txt;
    trackPQ.setTxtRect( SSvec[1].getFrRect(0,0), 'n', 'n' );

    fin.open("include/levels/perspect/persQuadList.txt");
    if( !fin ) { std::cout << "\nno persQuadList"; return false; }
    persQuad tmpPQ;
    unsigned int SSidx = 0;
    while( fin >> SSidx && tmpPQ.fromFile( fin, SSvec[SSidx] ) ) persQuadVec.push_back( tmpPQ );
    fin.close();

    fin.open("include/levels/perspect/persQuadAniList.txt");
    if( !fin ) { std::cout << "\nno persQuadAniList"; return false; }
    persQuadAni tmpPQA;
    while( fin >> SSidx && tmpPQA.fromFile( fin, SSvec[SSidx] ) ) persQuadAniVec.push_back( tmpPQA );
    fin.close();

    // persPack pointers
    ppPackVec.push_back( &boxA );
    ppPackVec.push_back( &flyBox );
    for( auto& car : carVec ) ppPackVec.push_back( &car );
    for( auto& pq : persQuadVec ) ppPackVec.push_back( &pq );
    for( auto& pqa : persQuadAniVec ) ppPackVec.push_back( &pqa );
    for( auto& pmx : persMatVec ) ppPackVec.push_back( &pmx );
    std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare );

    horizonLine[0].color = horizonLine[1].color = sf::Color::White;
    horizonLine[0].position.y = horizonLine[1].position.y = persPt::Yh;
 //   groundLine[0].color = groundLine[1].color = sf::Color::White;
 //   groundLine[0].position.y = groundLine[1].position.y = persPt::Yg;
    vanishPt[0].color = vanishPt[1].color = sf::Color::White;
    vanishPt[0].position.x = vanishPt[1].position.x = Level::winW/2.0f;
    vanishPt[0].position.y = persPt::Yh - 10.0f; vanishPt[1].position.y = persPt::Yh + 10.0f;

    // image from path
 /*   sf::Image trackImg;
    trackImg.create( 4000, 5000, sf::Color(50,50,50) );
    sf::Color laneClr[2] = { sf::Color::White, sf::Color::Red };
    for( unsigned int k = 0; k < 2; ++k )
    {
        const Leg* iter = pLegTrack[k];
        float x = pLegTrack[k]->x(0.0f), y = pLegTrack[k]->y(0.0f), v = 3.0f, s = 0.0f;
        while( iter == pLegTrack[k] )
        {
            for( unsigned int i = 0; i < 9; ++i )
                trackImg.setPixel( unsigned( x + 2000.0f ) + i%3, unsigned( 6000.0f - y ) + i/3, laneClr[k] );
            iter = iter->update( x, y, s, v, 1.0f );
        }
        while( iter && iter != pLegTrack[k] )
        {
            for( unsigned int i = 0; i < 9; ++i )
                trackImg.setPixel( unsigned( x + 2000.0f ) + i%3, unsigned( 6000.0f - y ) + i/3, laneClr[k] );
            iter = iter->update( x, y, s, v, 1.0f );
        }
    }
    trackImg.saveToFile("images/trackLane1.jpg");   */

    // ui is LAST
    // multi control list
    sf::Text label("Multi", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    sf::Vector2f hbPos(20.0f,10.0f), csPos(80.0f,10.0f);
    float dyButt = 27.0f;
    controlList.init( hbPos.x, hbPos.y, 50.0f, 25.0f, label );// box, fly, enviro
    controlList.persist = 3;
    controlList.pButtVec.push_back( &boxControl );
    controlList.pButtVec.push_back( &flyBoxControl );
    controlList.pButtVec.push_back( &enviroControl );
    controlList.pButtVec.push_back( &otherControl );
    button::RegisteredButtVec.push_back( &controlList );

    hbPos.y += dyButt; if( !makeBoxControl( hbPos, csPos ) ) return false;
    hbPos.y += dyButt; if( !makeFlyBoxControl( hbPos, csPos ) ) return false;
    hbPos.y += dyButt; if( !makeEnviroControl( hbPos, csPos ) ) return false;
    hbPos.y += dyButt; if( !makeOtherControl( hbPos, csPos ) ) return false;

    return true;
}

bool lvl_perspect::loadSpriteSheets()
{
    std::ifstream finList("include/levels/perspect/SSfileList.txt");
    if( !finList ) { std::cout << "\nno ssFileList"; return false; }
    std::string fileName;
    while( finList >> fileName )
    {
        fileName = "images/" + fileName;
        std::ifstream f_in( fileName.c_str() );
        if( !f_in ) { std::cout << "\nno " << fileName; return false; }
        SSvec.push_back( spriteSheet( f_in ) );
        f_in.close();
    }
    finList.close();
    std::cout << "\n#SS = " << SSvec.size();
    return true;
}

void lvl_perspect::makeBounds()
{
 //   road[0].init( -roadWidth/2.0f, 0.0f, persPt::Z0 );
//    road[1].init( roadWidth/2.0f, 0.0f, persPt::Z0 );

//    road[0].updateScreenPos(); roadVtx[0].position = road[0].screenPos();
//    road[1].updateScreenPos(); roadVtx[2].position = road[1].screenPos();
//    roadVtx[1].position.x = persPt::X0;
//    roadVtx[1].position.y = persPt::Yh;

 //   edgeLine[0].init( -xMax, 0.0f, persPt::Z0 );
 //   edgeLine[1].init( -xMax, 0.0f, persPt::z( persPt::Yh + dYh ) );
 //   edgeLine[2].init( xMax, 0.0f, persPt::z( persPt::Yh + dYh ) );
 //   edgeLine[3].init( xMax, 0.0f, persPt::Z0 );
 //   for( unsigned int i = 0; i < 4; ++i )
 //   {
 //       edgeLine[i].updateScreenPos();
 //       edgeVtx[i].position = edgeLine[i].screenPos();
 //       edgeVtx[i].color = sf::Color::Red;
 //   }

    // walls
    sideWall[0].init( -xMax, -yMax, persPt::Z0 );// at xs = -xMax: bottom front
    sideWall[1].init( -xMax, yMax, persPt::Z0 );// at xs = -xMax; top front
    sideWall[2].init( -xMax, yMax, persPt::z( persPt::Yh + dYh ) );// at xs = -xMax; top back
    sideWall[3].init( -xMax, -yMax, persPt::z( persPt::Yh + dYh ) );// at xs = -xMax; bottom back
    // at +xMax
    sideWall[4].init( xMax, -yMax, persPt::Z0 );// at xs = xMax: bottom front
    sideWall[5].init( xMax, yMax, persPt::Z0 );// at xs = xMax; top front
    sideWall[6].init( xMax, yMax, persPt::z( persPt::Yh + dYh ) );// at xs = xMax; top back
    sideWall[7].init( xMax, -yMax, persPt::z( persPt::Yh + dYh ) );// at xs = xMax; bottom back

 //   sf::Vertex wallVtx[8];// 2 x sf::Quad
    for( unsigned int i = 0; i < 8; ++i )
    {
        sideWall[i].updateScreenPos();
        wallVtx[i].position = sideWall[i].screenPos();
        wallVtx[i].color = sf::Color(200,0,0,100);
    }
    // back wall
    wallVtx[8].position = sideWall[2].screenPos();// -xMax, yMax
    wallVtx[9].position = sideWall[6].screenPos();// xMax, yMax
    wallVtx[10].position = sideWall[7].screenPos();// xMax, -yMax
    wallVtx[11].position = sideWall[3].screenPos();// -xMax, -yMax
    for( unsigned int i = 8; i < 12; ++i ) wallVtx[i].color = sf::Color(0,0,200,50);

    // floor
    const unsigned xi[8] = {0,3,7,4,1,2,6,5};
    for( unsigned int i = 0; i < 8; ++i ) floorCeilingVtx[i].position = sideWall[ xi[i] ].screenPos();
    for( auto& vtx : floorCeilingVtx ) vtx.color = sf::Color(50,50,50,150);

    horizonLine[0].position.x = sideWall[2].x;
    horizonLine[1].position.x = sideWall[6].x;
 //   horizonLine[0].position = sideWall[2].screenPos();
 //   horizonLine[1].position = sideWall[6].screenPos();

 //   groundLine[0].position.x = sideWall[1].x;
 //   groundLine[1].position.x = sideWall[5].x;
 //   groundLine[0].position = sideWall[1].screenPos();
 //   groundLine[1].position = sideWall[5].screenPos();

    // track
    track[0].init( trackPos );
    track[1].init( trackPos.x + trackDimX, trackPos.y, trackPos.z );
    track[2].init( trackPos.x + trackDimX, trackPos.y, trackPos.z + trackDimZ );
    track[3].init( trackPos.x, trackPos.y, trackPos.z + trackDimZ );
    for( unsigned int i = 0; i < 4; ++i )
    {
      //  track[i].setXvanish();
        track[i].updateScreenPos();
        trackQuad[i].position = track[i].screenPos();
    }
}

bool lvl_perspect::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Left ) moveX = -1;
        else if ( rEvent.key.code == sf::Keyboard::Right ) moveX = 1;
        else if ( rEvent.key.code == sf::Keyboard::Up ) { moveY = -1; }// moveSpeedY = 10.0f*moveSens; }
        else if ( rEvent.key.code == sf::Keyboard::Down ) { moveY = 1; }// moveSpeedY = 10.0f*moveSens; }
        else if ( rEvent.key.code == sf::Keyboard::Z ) moveZ = (1+moveZ)%2;
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::Left ) moveX = 0;
        else if ( rEvent.key.code == sf::Keyboard::Right ) moveX = 0;
        else if ( rEvent.key.code == sf::Keyboard::Up ) moveY = 0;
        else if ( rEvent.key.code == sf::Keyboard::Down ) moveY = 0;
    }

    return true;
}

void lvl_perspect::update( float dt )
{
    bool doSort = false;

    if( moveZ != 0 || moveX != 0 || moveY != 0 )
    {
        persPt::Xv += moveX*moveSpeedX*dt;
        Zv += moveZ*moveSpeedZ*dt;
        persPt::Zv = Zv;
        persPt::Yv += moveY*moveSpeedY*dt;

     //   if( moveZ == 1 && boxA.pos.z < persPt::Z0 ) boxA.pos.z = persPt::z( persPt::Yh + dYh );
        for( persPack* pPK : ppPackVec ) pPK->update();

        floorQuad.update();
        trackPQ.update();
        // update distances
        doSort = true;

        // lines and walls
         //   road[0].updateScreenPos(); roadVtx[0].position = road[0].screenPos();
         //   road[1].updateScreenPos(); roadVtx[2].position = road[1].screenPos();
         //   for( unsigned int i = 0; i < 4; ++i )
         //   {
            //    edgeLine[i].setXvanish();
         //       edgeLine[i].updateScreenPos();
         //       edgeVtx[i].position = edgeLine[i].screenPos();
         //   }
            for( unsigned int i = 0; i < 8; ++i )
            {
                sideWall[i].updateScreenPos();
                wallVtx[i].position = sideWall[i].screenPos();
            }
            // track
            for( unsigned int i = 0; i < 4; ++i )
            {
                track[i].updateScreenPos();
                trackQuad[i].position = track[i].screenPos();
            }
            // back wall
            wallVtx[8].position = sideWall[2].screenPos();// -xMax, yMax
            wallVtx[9].position = sideWall[6].screenPos();// xMax, yMax
            wallVtx[10].position = sideWall[7].screenPos();// xMax, -yMax
            wallVtx[11].position = sideWall[3].screenPos();// -xMax, -yMax

            // floor
            const unsigned xi[8] = {0,3,7,4,1,2,6,5};
            for( unsigned int i = 0; i < 8; ++i ) floorCeilingVtx[i].position = sideWall[ xi[i] ].screenPos();

   //     if( moveY != 0 ) groundLine[0].position.y = groundLine[1].position.y = persPt::Yg + persPt::Yv;

        if( moveX != 0 )
        {
            horizonLine[0].position.x = sideWall[2].x; horizonLine[1].position.x = sideWall[6].x;
    //        groundLine[0].position.x = sideWall[1].x; groundLine[1].position.x = sideWall[5].x;
        }
    }

    // flyBox
    if( flyBox.v.z + moveZ*moveSpeedZ < 0.0f && flyBox.pos.z < persPt::Z0 ) { flyBox.setPosition( flyBox.pos.x, flyBox.pos.y, persPt::z( persPt::Yh + dYh ) ); }
    if( gravity.y != 0.0f ) flyBox.v += gravity*dt;
    flyBox.updateMove( dt );
//    if( !moveZ && flyBox.v.dot( flyBox.v ) > 0.1f )
    if( flyBox.v.dot( flyBox.v ) > 0.1f )
    {
        flyBox.keepInBounds( -xMax, xMax, -yMax, yMax, persPt::Z0, persPt::z( persPt::Yh + dYh ) );
        doSort = true;
    }

    if( pathButt.sel )
    {
    //    if( LAboxA.pLeg ) updateLAbox( LAboxA, boxA, dt );
        for( unsigned int i = 0; i < carLAvec.size(); ++i ) updateLAbox( carLAvec[i], carVec[i], dt );
        doSort = true;
    }

    if( doSort ) std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare );

    // spin cube
    if( persMatVec.size() > 3 )// rotate
        persMatVec[3].rotate_axis( vec3f(0.0f,1.0f,0.0f), 0.03f );

    return;
}

void lvl_perspect::draw( sf::RenderTarget& RT ) const
{
    RT.draw( horizonLine, 2, sf::Lines );
    RT.draw( vanishPt, 2, sf::Lines );
    RT.draw( wallVtx, 12, sf::Quads );
    RT.draw( floorCeilingVtx, 8, sf::Quads );
//    RT.draw( trackQuad, 4, sf::Quads, &floorTxt );
//    RT.draw( trackQuad, 4, sf::Quads, &SSvec[1].txt );
    trackPQ.draw(RT);
    floorQuad.draw(RT);
 //   RT.draw( roadVtx, 3, sf::LinesStrip );
 //   RT.draw( edgeVtx, 4, sf::LinesStrip );


//    RT.draw( &vtxVec[0], vtxVec.size(), sf::Lines );// persPtVec points
//    for( const auto& pp : persPackVec ) pp.draw(RT);
    for( const auto& ppk : ppPackVec ) ppk->draw(RT);
//    RT.draw( groundLine, 2, sf::Lines );
    return;
}

void lvl_perspect::cleanup()
{
    if( pLegTrack[0] ) { std::cout << "\n# legs destroyed = " << destroyPath( pLegTrack[0] ); }
    if( pLegTrack[1] ) { std::cout << "\n# legs destroyed = " << destroyPath( pLegTrack[1] ); }
}

bool lvl_perspect::makeBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    boxControl.init( hbPos, csPos, sf::Vector2f(300.0f,120.0f), "BOX" );
    boxControl.pButtVec.push_back( &boxMS );

    std::function<void(void)> pReSort = [this](){ std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare ); };
    cloneButt.init( csPos.x + 250.0f, csPos.y + 38.0f, 40.0f, 20.0f, [this,pReSort](){ if( boxVec.size() < boxVec.capacity() ) { boxVec.push_back( boxA ); ppPackVec.push_back( &boxVec.back() ); pReSort(); } }, "Clone" );
    cloneButt.mode = 'I';
    boxControl.pButtVec.push_back( &cloneButt );
    pathButt.init( csPos.x + 202.0f, csPos.y + 38.0f, 40.0f, 20.0f, nullptr, "Path" );
    boxControl.pButtVec.push_back( &pathButt );

    std::ifstream fin( "include/levels/perspect/boxMS.txt" );
    if( !fin ) { std::cout << "\n no boxMS data "; return false; }
    sf::Vector2f ofst(10.0f,10.0f);
    boxMS.init( csPos + ofst, &boxMsStrip, fin );
    fin.close();

    if( boxMS.sdataVec.size() != 10 ) { std::cout << "\nbad boxMS data"; return false; }
    stripData* pSD = &boxMS.sdataVec.front();
    pSD[0].pSetFunc = [this,pReSort](float x){ boxA.init( x, boxA.pos.y, boxA.pos.z, boxA.dimX, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); pReSort(); };// posX
    pSD[1].pSetFunc = [this,pReSort](float x){ boxA.init( boxA.pos.x, x, boxA.pos.z, boxA.dimX, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); pReSort(); };// posY
    pSD[2].pSetFunc = [this,pReSort](float x){ boxA.init( boxA.pos.x, boxA.pos.y, x, boxA.dimX, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); pReSort(); };// posZ
    pSD[3].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, boxA.pos.z, x, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); };// dimX
    pSD[4].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, boxA.pos.z, boxA.dimX, x, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); };// dimY
    pSD[5].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, boxA.pos.z, boxA.dimX, boxA.dimY, x, boxA.vtx[0].color, boxA.drawFaces ); };// dimZ
    pSD[6].pSetFunc = [this](float x)// pitch
    {
        boxA.Ttp = vec3f(0.0f,1.0f,x);// top
        boxA.Ttp = boxA.Ttp.normalize();
        boxA.Tbk = vec3f(0.0f,-boxA.Ttp.z,boxA.Ttp.y);// back
        boxA.Trt = boxA.Ttp.cross( boxA.Tbk );

        boxA.Tbt = -boxA.Ttp; boxA.Tfr = -boxA.Tbk;  boxA.Tlt = -boxA.Trt;
        boxA.setPosition( boxA.pos );
    };
    pSD[7].pSetFunc = [this](float x)// yaw
    {
        boxA.Tbk = vec3f(0.0f,-boxA.Ttp.z,boxA.Ttp.y);// back
        boxA.Tbk = boxA.Tbk*cosf(x) + vec3f(1.0f,0.0f,0.0f)*sinf(x);
        boxA.Trt = boxA.Ttp.cross( boxA.Tbk );

        boxA.Tfr = -boxA.Tbk;  boxA.Tlt = -boxA.Trt;
        boxA.setPosition( boxA.pos );
    };
    pSD[8].pSetFunc = [this](float x)// roll
    {
        boxA.Ttp = boxA.Ttp.rotate_axis( boxA.Tfr, x );
        boxA.Trt = boxA.Ttp.cross( boxA.Tbk );
        boxA.Tbt = -boxA.Ttp;
        boxA.Tlt = -boxA.Trt;
        boxA.setPosition( boxA.pos );
    };
    pSD[9].pSetFunc = [this](float x) { LAboxA.v = x; }; // path speed
    boxMsStrip.reInit( *pSD );// posX
    boxA.init( pSD[0].xCurr, pSD[1].xCurr, pSD[2].xCurr, pSD[3].xCurr, pSD[4].xCurr, pSD[5].xCurr, boxA.vtx[0].color, boxA.drawFaces );
 //   updateLAbox( LAboxA, boxA, 0.0f );

    return true;
}

bool lvl_perspect::makeFlyBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    flyBoxControl.init( hbPos, csPos, sf::Vector2f(300.0f,120.0f), "fly" );
    flyBoxControl.pButtVec.push_back( &flyBoxMS );

    std::ifstream fin( "include/levels/perspect/flyBoxMS.txt" );
    if( !fin ) { std::cout << "\n no flyBoxMS data "; return false; }
    sf::Vector2f ofst(10.0f,10.0f);
    flyBoxMS.init( csPos + ofst, &flyBoxMsStrip, fin );
    fin.close();

    if( flyBoxMS.sdataVec.size() != 7 ) { std::cout << "\nbad flyBoxMS data"; return false; }
    stripData* pSD = &flyBoxMS.sdataVec.front();

    std::function<void(void)> pReSort = [this](){ std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare ); };
    pSD[0].pSetFunc = [this,pReSort](float x){ flyBox.setPosition( x, flyBox.pos.y, flyBox.pos.z ); pReSort(); to_SF_string( posXmsg, x ); };// posX
    pSD[1].pSetFunc = [this,pReSort](float x){ flyBox.setPosition( flyBox.pos.x, x, flyBox.pos.z ); pReSort(); to_SF_string( posYmsg, x ); };// posY
    pSD[2].pSetFunc = [this,pReSort](float x){ flyBox.setPosition( flyBox.pos.x, flyBox.pos.y, x ); pReSort(); to_SF_string( posZmsg, x ); };// posZ
    pSD[3].pSetFunc = [this](float x){ flyBox.v.x = x; };// flyVel.x
    pSD[4].pSetFunc = [this](float x){ flyBox.v.y = x; };// flyVel.y
    pSD[5].pSetFunc = [this](float x){ flyBox.v.z = x; };// flyVel.z
    pSD[6].pSetFunc = [this](float x){ gravity.y = x; };// gravity

    flyBoxMsStrip.reInit( *pSD );// posX
    flyBox.init( pSD[0].xCurr, pSD[1].xCurr, pSD[2].xCurr, flyBox.dimX, flyBox.dimY, flyBox.dimZ, flyBox.vtx[0].color, flyBox.drawFaces );
    std::cout << "\nflyBox.pos.z = " << flyBox.pos.z << " xCurr = " << pSD[2].xCurr;

    posXmsg.setFont( *button::pFont );
    posXmsg.setCharacterSize( 12 );
    posXmsg.setColor( sf::Color::White );
    posXmsg.setPosition( csPos.x + 80.0f, csPos.y + 40.0f );
    to_SF_string( posXmsg, pSD[0].xCurr );
    posZmsg = posYmsg = posXmsg;
    posYmsg.setPosition( csPos.x + 140.0f, csPos.y + 40.0f );
    to_SF_string( posYmsg, pSD[1].xCurr );
    posZmsg.setPosition( csPos.x + 200.0f, csPos.y + 40.0f );
    to_SF_string( posZmsg, pSD[2].xCurr );

    flyBoxControl.updateAll = [this](  )
    {
        to_SF_string( posZmsg, flyBox.pos.z );
        if( flyBox.v.x != 0.0f ) to_SF_string( posXmsg, flyBox.pos.x );
        if( flyBox.v.y != 0.0f ) to_SF_string( posYmsg, flyBox.pos.y );
    };
    flyBoxControl.drawAll = [this]( sf::RenderTarget& RT ){ RT.draw( posXmsg ); RT.draw( posYmsg ); RT.draw( posZmsg ); };

    return true;
}

/*
bool lvl_perspect::makeGunControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    gunControl.init( hbPos, csPos, sf::Vector2f(300.0f,120.0f), "AA gun" );
    gunControl.pButtVec.push_back( &gunMS );

    std::ifstream fin("include/levels/perspect/gunBox_init.txt");
    if( !fin ) { std::cout << "\nno boxA_init"; return false; }
    if( !gunBox.fromFile( fin, true ) ) { std::cout << "\nno gunBox.fromFile fail"; return false; }
    fin.close();
    fin.open( "images/AAgunSSdata.txt" );
    if( !fin ) { std::cout << "no AAgunSS data\n"; return false; }
    gunSS.init(fin);
    fin.close();
    gunBox.pTxt = &gunSS.txt;
    gunBox.setTxtRect( gunSS.getFrRect(0,0), vec3f(0.0f,0.0f,-1.0f) );// front face = brick 25
    gunBox.setTxtRect( gunSS.getFrRect(1,0), vec3f(0.0f,0.0f,1.0f) );// back face = water 11
    gunBox.setTxtRect( gunSS.getFrRect(2,0), vec3f(0.0f,1.0f,0.0f) );// top face = grass 10
    gunBox.setTxtRect( gunSS.getFrRect(3,0), vec3f(0.0f,-1.0f,0.0f) );// bottom face = rocks+dirt 8
    gunBox.setTxtRect( gunSS.getFrRect(4,0), vec3f(-1.0f,0.0f,0.0f) );// Left face = stone 24
    gunBox.setTxtRect( gunSS.getFrRect(5,0), vec3f(1.0f,0.0f,0.0f), 'F', 'X' );// Right face = stripes 55

    fin.open( "include/levels/perspect/gunMS.txt" );
    if( !fin ) { std::cout << "\n no gunMS data "; return false; }
    sf::Vector2f ofst(10.0f,10.0f);
    gunMS.init( csPos + ofst, &gunMsStrip, fin );
    fin.close();

    if( gunMS.sdataVec.size() != 4 ) { std::cout << "\nbad gunMS data"; return false; }
    stripData* pSD = &gunMS.sdataVec.front();
    std::function<void(void)> pReSort = [this](){ std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare ); };
    pSD[0].pSetFunc = [this,pReSort](float x){ gunBox.setPosition( x, gunBox.pos.y, gunBox.pos.z ); pReSort(); };// posX
    pSD[1].pSetFunc = [this,pReSort](float x){ gunBox.setPosition( gunBox.pos.x, x, gunBox.pos.z ); pReSort(); };// posY
    pSD[2].pSetFunc = [this,pReSort](float x){ gunBox.setPosition( gunBox.pos.x, gunBox.pos.y, x ); pReSort(); };// posZ
    pSD[3].pSetFunc = [this](float x)// yaw
    {
        gunBox.Tbk = vec3f(0.0f,-gunBox.Ttp.z,gunBox.Ttp.y);// back
        gunBox.Tbk = gunBox.Tbk*cosf(x) + vec3f(1.0f,0.0f,0.0f)*sinf(x);
        gunBox.Trt = gunBox.Ttp.cross( gunBox.Tbk );

        gunBox.Tfr = -gunBox.Tbk; gunBox.Tlt = -gunBox.Trt;
        gunBox.setPosition( gunBox.pos );
    };

    gunMsStrip.reInit( *pSD );// posX
    gunBox.setPosition( pSD[0].xCurr, pSD[1].xCurr, pSD[2].xCurr );

    return true;
}   */

bool lvl_perspect::makeEnviroControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    enviroControl.init( hbPos, csPos, sf::Vector2f(370.0f,134.0f), "enviro" );
    enviroControl.pButtVec.push_back( &enviroMS );

    std::ifstream fin( "include/levels/perspect/enviroMS.txt" );
    if( !fin ) { std::cout << "\n no enviroMS data "; return false; }
    sf::Vector2f ofst(10.0f,10.0f);
    enviroMS.init( csPos + ofst, &enviroMsStrip, fin );
    fin.close();

    enviroMsStrip.setPosition( {csPos.x + 10.0f, csPos.y + 90.0f} );

    if( enviroMS.sdataVec.size() != 7 ) { std::cout << "\nbad enviroMS data"; return false; }
    stripData* pSD = &enviroMS.sdataVec.front();
 //   enviroMsStrip.reInit( *pSD );// posX

    std::function<void(void)> pFset = [this]()
    {
        for( auto& ppk : ppPackVec ) ppk->setPosition( ppk->pos );
   //     groundLine[0].position.y = groundLine[1].position.y = persPt::Yg + persPt::Yv;
        horizonLine[0].position.y = horizonLine[1].position.y = persPt::Yh;
   //     avbPt[0].init( vbPos + axis ); avbPt[1].init( vbPos ); avbPt[2].init( vbPos + vecB );
        makeBounds();
    };

    pSD[0].pSetFunc = [this,pFset](float x){ persPt::Yg = flyFpos.z = x; pFset(); };// Yg
    persPt::Yg = flyFpos.z = pSD[0].xCurr;
    pSD[1].pSetFunc = [this,pFset](float x){ persPt::Yh = x; pFset(); };// Yh
    persPt::Yh = pSD[1].xCurr;
    pSD[2].pSetFunc = [this](float x){ xMax = x; makeBounds(); };// xMax
    xMax = pSD[2].xCurr;
    pSD[3].pSetFunc = [this](float x){ yMax = x; makeBounds(); };// yMax
    yMax = pSD[3].xCurr;
    pSD[4].pSetFunc = [this](float x){ dYh = x; makeBounds(); };// dYh
 //   pSD[4].pSetFunc = [this](float x){ dYh = persPt::Y_0( x ) - persPt::Yh; makeBounds(); };// dYh
 //   dYh = pSD[4].xCurr;
 //   dYh = 50.0f;
    pSD[5].pSetFunc = [this](float x){ moveSens = x; };// fly_jButt input scale
    moveSens = pSD[5].xCurr;
    pSD[6].pSetFunc = [this](float x){ moveSpeedY = x; };// moveSpeedY
    moveSpeedY = pSD[6].xCurr;
    pFset();
    enviroMsStrip.reInit( *pSD );

    // other
    fly_jButt.init( 50.0f, 6.0f, csPos.x + 310.0f, csPos.y + 60.0f );
    fly_jButt.pFunc_v2d = [this]( vec2f disp ) { moveSpeedX = moveSens*disp.x; moveSpeedZ = -moveSens*disp.y; };
    fly_jButt.writeOnDrop = true;
    enviroControl.pButtVec.push_back( &fly_jButt );

//    moveLtButt.init( csPos.x + 108.0f, csPos.y + 38.0f, 50.0f, 24.0f, [this](){ moveSpeedX = 10.0f*moveSens; moveX = moveLtButt.sel ? -1 : 0; }, "<--X" ); moveLtButt.mode = 'I';
    moveLtButt.init( csPos.x + 108.0f, csPos.y + 38.0f, 50.0f, 24.0f, nullptr, "<--X" ); moveLtButt.mode = 'I';
    enviroControl.pButtVec.push_back( &moveLtButt );
//    moveRtButt.init( csPos.x + 168.0f, csPos.y + 38.0f, 50.0f, 24.0f, [this](){ moveSpeedX = 10.0f*moveSens; moveX = moveRtButt.sel ? 1 : 0; }, "X-->" ); moveRtButt.mode = 'I';
    moveRtButt.init( csPos.x + 168.0f, csPos.y + 38.0f, 50.0f, 24.0f, nullptr, "X-->" ); moveRtButt.mode = 'I';
    enviroControl.pButtVec.push_back( &moveRtButt );

    // display camera position
    XvTxt.setFont( *button::pFont );
    XvTxt.setCharacterSize( 12 );
    XvTxt.setColor( sf::Color::White );
    ZvTxt = YvTxt = XvTxt;
    XvTxt.setPosition( csPos.x + 10.0f, csPos.y + 60.0f );
    to_SF_string( XvTxt, persPt::Xv );
    YvTxt.setPosition( csPos.x + 90.0f, csPos.y + 60.0f );
    to_SF_string( YvTxt, persPt::Yv );
    ZvTxt.setPosition( csPos.x + 170.0f, csPos.y + 60.0f );
    to_SF_string( ZvTxt, persPt::Zv );

    enviroControl.updateAll = [this,pFset]()
    {
     //   if( !( moveLtButt.sel || moveRtButt.sel ) ) moveX = fly_jButt.atRest ? 0 : 1;
        moveX = fly_jButt.atRest ? 0 : 1;
        if( moveLtButt.sel || moveRtButt.sel )// rotate world
        {
            float dAng = dAngPolar;
            if( moveLtButt.sel ) dAng *= -1.0f;
            persPt::angPolar += dAng;
            moveX = moveZ = 1;
        }
        // reset angPolar = 0.0f with right click on either button
        if( button::clickEvent_Rt() == 1 && (moveLtButt.mseOver || moveRtButt.mseOver) ){ persPt::angPolar = 0.0f; pFset(); }

        moveZ = fly_jButt.atRest ? 0 : 1;
        if( fly_jButt.mseOver )// joyButton is held
        {
            if( button::mseDnLt ) moveY = 1;
            else if( button::mseDnRt ) moveY = -1;
            else moveY = 0;
        //    if( fly_jButt.sel )// not in dead zone
        //    {
                to_SF_string( XvTxt, persPt::Xv );
                to_SF_string( YvTxt, persPt::Yv );
                to_SF_string( ZvTxt, persPt::Zv );
         //   }
        }
    };

    enviroControl.drawAll = [this](sf::RenderTarget& RT)
    {
        RT.draw( XvTxt );
        RT.draw( YvTxt );
        RT.draw( ZvTxt );
    };

    return true;
}

bool lvl_perspect::makeOtherControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    otherControl.init( hbPos, csPos, sf::Vector2f(300.0f,120.0f), "other" );

    // loading boxes
    saveToFileButt.init( csPos.x + 50.0f, csPos.y + 40.0f, 40.0f, 20.0f, nullptr, "Save" );
    saveToFileButt.pHitFunc = [this]()
    {
        std::ofstream fout("include/levels/perspect/savedBoxes.txt");
        for( const auto& bx : boxVec ) bx.toFile( fout );
    };
    otherControl.pButtVec.push_back( &saveToFileButt );

    loadFromFileButt.init( csPos.x + 150.0f, csPos.y + 40.0f, 40.0f, 20.0f, nullptr, "Load" );
    loadFromFileButt.pHitFunc = [this]()
    {
        std::ifstream fin("include/levels/perspect/savedBuildings.txt");
        if( !fin ) { std::cout << "\nno savedBoxes"; return; }
        boxVec.clear();
        unsigned int cap; fin >> cap;
        boxVec.reserve( cap );
        persBox tmpBox;
        unsigned int SSidx = 0;
        while( fin >> SSidx && tmpBox.fromFile( fin, SSvec[SSidx] ) ) boxVec.push_back( tmpBox );

        for( persBox& pbx : boxVec ) ppPackVec.push_back( &pbx );
        std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare );
    };
    /*
    loadFromFileButt.pHitFunc = [this]()
    {
        std::ifstream fin("include/levels/perspect/savedBoxes.txt");
        if( !fin ) { std::cout << "\nno savedBoxes"; return; }
        boxVec.clear();
        unsigned int cap; fin >> cap;
        boxVec.reserve( cap );
        persBox tmpBox;
        while( tmpBox.fromFile(fin) ) boxVec.push_back( tmpBox );

        for( persBox& pbx : boxVec ) ppPackVec.push_back( &pbx );
        std::sort( ppPackVec.begin(), ppPackVec.end(), persPack::compare );
    };
    */
    otherControl.pButtVec.push_back( &loadFromFileButt );

    return true;
}

// non member
void setTxtRect( sf::Vertex* vtx, sf::IntRect srcRect )
{
    vtx[0].texCoords.x = srcRect.left;// up lt
    vtx[0].texCoords.y = srcRect.top;
    vtx[1].texCoords.x = srcRect.left + srcRect.width;// up rt
    vtx[1].texCoords.y = srcRect.top;
    vtx[2].texCoords.x = srcRect.left + srcRect.width;// dn rt
    vtx[2].texCoords.y = srcRect.top + srcRect.height;
    vtx[3].texCoords.x = srcRect.left;// dn lt
    vtx[3].texCoords.y = srcRect.top + srcRect.height;
}

void updateLAbox( legAdapter& rLA, persBox& rCar, float dt )
{
    rLA.pLeg = rLA.pLeg->update( rLA.x, rLA.y, rLA.s, rLA.v, dt );
    rLA.pLeg->T( rLA.s, rCar.Tfr.x, rCar.Tfr.z );
    if( rLA.v < 0.0f ) { rCar.Tfr.x *= -1.0f; rCar.Tfr.z *= -1.0f; };// face other way
    rCar.Tbk = -rCar.Tfr;
    rCar.Trt = rCar.Ttp.cross( rCar.Tbk );
    rCar.Tlt = -rCar.Trt;
    rCar.setPosition( rLA.x, rCar.pos.y, rLA.y );
}
