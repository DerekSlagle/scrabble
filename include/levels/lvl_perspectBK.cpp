#include "lvl_perspect.h"

// lvl_perspect mfs
bool lvl_perspect::init()
{
    Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,0,0);
    button::setHoverBoxColor( Level::clearColor );

    // load some paths
    std::ifstream finList("include/levels/perspect/persPtFileList.txt");
    if( !finList ) { std::cout << "\nno persPtFileList"; return false; }
    std::string fileName;
    while( finList >> fileName )
    {
        fileName = "include/levels/perspect/" + fileName;
        std::ifstream f_in( fileName.c_str() );
        if( !f_in ) { std::cout << "\nno " << fileName; return false; }
        f_in >> persPt::Yg >> persPt::Yh;
        float xs, ys, y0;
        while( f_in >> xs >> ys >> y0 )
            persPtVec.push_back( persPt( xs, ys, y0 ) );

        f_in.close();
    }

    finList.close();

    vtxVec.resize( persPtVec.size() );
    std::cout << "\npersPtVec.size() = " << persPtVec.size();
    for( unsigned int i = 0; i< persPtVec.size(); ++i )
    {
        vtxVec[i].position.x = persPtVec[i].x + Level::winW/2.0f;
        vtxVec[i].position.y = persPtVec[i].y;
        vtxVec[i].color = sf::Color::Green;
    }

    viewPos.x = persPt::X0 = Level::winW/2.0f;
    viewPos.y = persPt::Yh;
    viewPos.z = persPt::Z0 = 1000.0f;
    makeBounds();// road, edgeLine, walls

    //          x       y     y0      dimX    dimY   dimZ
 //   boxA.init( 100.0f, 100.0f, 300.0f, 100.0f, 120.0f, 100.0f, Yh, Yg, sf::Color::Red, false );

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


    std::ifstream fin("include/levels/perspect/boxA_init.txt");
    if( !fin ) { std::cout << "\nno boxA_init"; return false; }
    if( !boxA.fromFile( fin, true ) ) { std::cout << "\nno boxA.fromFile fail"; return false; }
    fin.close();
    hbPos.y += dyButt;
    if( !makeBoxControl( hbPos, csPos ) ) return false;
 //   button::RegisteredButtVec.push_back( &boxControl );
    // more boxA stuff
 //   fin.open( "images/tiles1_SSdata.txt" );
    fin.open( "images/cars/minivan_SSdata.txt" );
    if( !fin ) { std::cout << "no faceSS data\n"; return false; }
 //   std::string imgFname; fin >> imgFname;
 //   if( !faceSS.txt.loadFromFile( imgFname.c_str() ) ) { std::cout << "faceSS.txt load fail\n"; return false; }
    faceSS.init(fin);
    fin.close();
  //  boxA.drawFaces = true;// too late
    boxA.pTxt = &faceSS.txt;
    boxA.setTxtRect( faceSS.getFrRect(0,0), vec3f(0.0f,0.0f,1.0f), 'R', '1' );// front face = brick 25
    boxA.setTxtRect( faceSS.getFrRect(1,0), vec3f(0.0f,0.0f,-1.0f) );// back face = water 11
    boxA.setTxtRect( faceSS.getFrRect(2,0), vec3f(0.0f,-1.0f,0.0f) );// top face = grass 10
    boxA.setTxtRect( faceSS.getFrRect(4,0), vec3f(0.0f,1.0f,0.0f) );// bottom face = rocks+dirt 8
    boxA.setTxtRect( faceSS.getFrRect(3,0), vec3f(-1.0f,0.0f,0.0f) );// Left face = stone 24
    boxA.setTxtRect( faceSS.getFrRect(3,0), vec3f(1.0f,0.0f,0.0f) );// Right face = stripes 55


    flyIpos.x = -100.0f; flyIpos.y = 100.0f; flyIpos.z = persPt::Yh + 5.0f;
    flyFpos.x = -100.0f; flyFpos.y = 100.0f; flyFpos.z = persPt::Yg;
    flyBox.init( flyIpos.x, flyIpos.y, flyIpos.z, 100.0f, 80.0f, 100.0f, sf::Color::Yellow, false );// fly over scene
    flySpeed = 0.0f;
    vec3f dPos = flyFpos - flyIpos;
    flyBox.v = flySpeed*dPos/dPos.mag();
    hbPos.y += dyButt;
    if( !makeFlyBoxControl( hbPos, csPos ) ) return false;
//    button::RegisteredButtVec.push_back( &flyBoxControl );
    hbPos.y += dyButt;
    if( !makeEnviroControl( hbPos, csPos ) ) return false;
    hbPos.y += dyButt;
    if( !makeOtherControl( hbPos, csPos ) ) return false;
 //   button::RegisteredButtVec.push_back( &enviroControl );

//    viewPos.x = Level::winW/2.0f;
  //  viewPos.y = persPt::Yh;
//    viewPos.z = 1000.0f;

    horizonLine[0].color = horizonLine[1].color = sf::Color::White;
 //   horizonLine[0].position.x = 0.0f;
    horizonLine[0].position.y = horizonLine[1].position.y = persPt::Yh;
 //   horizonLine[1].position.x = Level::winW;
    groundLine[0].color = groundLine[1].color = sf::Color::White;
 //   groundLine[0].position.x = 0.0f;
    groundLine[0].position.y = groundLine[1].position.y = persPt::Yg;
//    groundLine[1].position.x = Level::winW;
    vanishPt[0].color = vanishPt[1].color = sf::Color::White;
    vanishPt[0].position.x = vanishPt[1].position.x = Level::winW/2.0f;
    vanishPt[0].position.y = persPt::Yh - 10.0f; vanishPt[1].position.y = persPt::Yh + 10.0f;

    return true;
}

void lvl_perspect::makeBounds()
{
    road[0].init( -roadWidth/2.0f, 0.0f, persPt::Yg );
    road[0].setXvanish( Xv );
    road[1].init( roadWidth/2.0f, 0.0f, persPt::Yg );
    road[1].setXvanish( Xv );

    roadVtx[0].position.x = road[0].x + persPt::X0;
    roadVtx[0].position.y = road[0].y;
    roadVtx[1].position.x = persPt::X0;
    roadVtx[1].position.y = persPt::Yh;
    roadVtx[2].position.x = road[1].x + persPt::X0;
    roadVtx[2].position.y = road[1].y;

    edgeLine[0].init( -xMax, 0.0f, persPt::Yg );
    edgeLine[1].init( -xMax, 0.0f, persPt::Yh + dYh );
    edgeLine[2].init( xMax, 0.0f, persPt::Yh + dYh );
    edgeLine[3].init( xMax, 0.0f, persPt::Yg );
    for( unsigned int i = 0; i < 4; ++i )
    {
        edgeLine[i].setXvanish( Xv );
        edgeVtx[i].position.x = edgeLine[i].x + persPt::X0; edgeVtx[i].position.y = edgeLine[i].y;
        edgeVtx[i].color = sf::Color::Red;
    }

    // walls
    sideWall[0].init( -xMax, -yMax, persPt::Yg );// at xs = -xMax: bottom front
    sideWall[1].init( -xMax, yMax, persPt::Yg );// at xs = -xMax; top front
    sideWall[2].init( -xMax, yMax, persPt::Yh + dYh );// at xs = -xMax; top back
    sideWall[3].init( -xMax, -yMax, persPt::Yh + dYh );// at xs = -xMax; bottom back
    // at +xMax
    sideWall[4].init( xMax, -yMax, persPt::Yg );// at xs = xMax: bottom front
    sideWall[5].init( xMax, yMax, persPt::Yg );// at xs = xMax; top front
    sideWall[6].init( xMax, yMax, persPt::Yh + dYh );// at xs = xMax; top back
    sideWall[7].init( xMax, -yMax, persPt::Yh + dYh );// at xs = xMax; bottom back

 //   sf::Vertex wallVtx[8];// 2 x sf::Quad
    for( unsigned int i = 0; i < 8; ++i )
    {
        sideWall[i].setXvanish( Xv );
        wallVtx[i].position.x = sideWall[i].x + persPt::X0; wallVtx[i].position.y = sideWall[i].y;
        wallVtx[i].color = sf::Color(200,0,0,100);
    }
    // back wall
    wallVtx[8].position.x = sideWall[2].x + persPt::X0; wallVtx[8].position.y = sideWall[2].y;// -xMax, yMax
    wallVtx[9].position.x = sideWall[6].x + persPt::X0; wallVtx[9].position.y = sideWall[6].y;// xMax, yMax
    wallVtx[10].position.x = sideWall[7].x + persPt::X0; wallVtx[10].position.y = sideWall[7].y;// xMax, -yMax
    wallVtx[11].position.x = sideWall[3].x + persPt::X0; wallVtx[11].position.y = sideWall[3].y;// -xMax, -yMax
    for( unsigned int i = 8; i < 12; ++i ) wallVtx[i].color = sf::Color(0,0,200,50);

    // floor
    floorCeilingVtx[0].position.x = sideWall[0].x + persPt::X0; floorCeilingVtx[0].position.y = sideWall[0].y;//-xmax, Yg
    floorCeilingVtx[1].position.x = sideWall[3].x + persPt::X0; floorCeilingVtx[1].position.y = sideWall[3].y;//-xmax, Yh +
    floorCeilingVtx[2].position.x = sideWall[7].x + persPt::X0; floorCeilingVtx[2].position.y = sideWall[7].y;// xmax, Yh +
    floorCeilingVtx[3].position.x = sideWall[4].x + persPt::X0; floorCeilingVtx[3].position.y = sideWall[4].y;// xmax, Yg
    // ceiling
    floorCeilingVtx[4].position.x = sideWall[1].x + persPt::X0; floorCeilingVtx[4].position.y = sideWall[1].y;//-xmax, Yg
    floorCeilingVtx[5].position.x = sideWall[2].x + persPt::X0; floorCeilingVtx[5].position.y = sideWall[2].y;//-xmax, Yh +
    floorCeilingVtx[6].position.x = sideWall[6].x + persPt::X0; floorCeilingVtx[6].position.y = sideWall[6].y;// xmax, Yh +
    floorCeilingVtx[7].position.x = sideWall[5].x + persPt::X0; floorCeilingVtx[7].position.y = sideWall[5].y;// xmax, Yg
    for( auto& vtx : floorCeilingVtx ) vtx.color = sf::Color(50,50,50,150);

    horizonLine[0].position.x = sideWall[2].x + persPt::X0; horizonLine[1].position.x = sideWall[6].x + persPt::X0;
    groundLine[0].position.x = sideWall[1].x + persPt::X0; groundLine[1].position.x = sideWall[5].x + persPt::X0;
}

bool lvl_perspect::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::Left ) moveX = -1;
        else if ( rEvent.key.code == sf::Keyboard::Right ) moveX = 1;
        else if ( rEvent.key.code == sf::Keyboard::Up ) moveZ = -1;
        else if ( rEvent.key.code == sf::Keyboard::Down ) moveZ = 1;
        else if ( rEvent.key.code == sf::Keyboard::Z ) moveZ = (1+moveZ)%2;
    }
    else if ( rEvent.type == sf::Event::KeyReleased )
    {
        if ( rEvent.key.code == sf::Keyboard::Left ) moveX = 0;
        else if ( rEvent.key.code == sf::Keyboard::Right ) moveX = 0;
        else if ( rEvent.key.code == sf::Keyboard::Up ) moveZ = 0;
        else if ( rEvent.key.code == sf::Keyboard::Down ) moveZ = 0;
    }

    return true;
}

void lvl_perspect::update( float dt )
{
    float bxAposX = boxA.pos.x;
    float flyBoxPosX = flyBox.pos.x;

    if( moveZ != 0 || moveX != 0 )
    {
        Xv += moveX*moveSpeedX*dt;
        Zv += moveZ*moveSpeedZ*dt;

        if( moveZ == 1 && boxA.pos.z > persPt::Yg ) boxA.pos.z = persPt::Yh + dYh;

        boxA.setPosition( boxA.pos.x - Xv, boxA.pos.y, boxA.pos.z + persPt::zFact( boxA.pos.z )*moveZ*moveSpeedZ*dt );
        boxA.pos.x = bxAposX;
        flyBox.setPosition( flyBox.pos.x - Xv, flyBox.pos.y, flyBox.pos.z + persPt::zFact( flyBox.pos.z )*moveZ*moveSpeedZ*dt );
   //     float Y0 = flyBox.Y_0( flyBox.pos.z ) + flyBox.dY0dz()*moveZ*moveSpeedZ*dt;
   //     flyBox.setPosition( flyBox.pos.x - Xv, flyBox.pos.y, Y0 );
        flyBox.pos.x = flyBoxPosX;

        for( auto& bx : boxVec )
        {
            if( moveZ == 1 && bx.pos.z > persPt::Yg ) bx.pos.z = persPt::Yh + dYh;
            float bxposX = bx.pos.x;
            bx.setPosition( bx.pos.x - Xv, bx.pos.y, bx.pos.z + persPt::zFact( bx.pos.z )*moveZ*moveSpeedZ*dt );
            bx.pos.x = bxposX;
        }

        for(  unsigned int i = 0; i< persPtVec.size(); ++i )
        {
            float Xs = persPtVec[i].xs;
            persPtVec[i].init( Xs - Xv, persPtVec[i].ys, persPtVec[i].y0 + persPt::zFact( persPtVec[i].y0 )*moveZ*moveSpeedZ*dt );
            persPtVec[i].xs = Xs;
            vtxVec[i].position.x = persPtVec[i].x + Level::winW/2.0f;
            vtxVec[i].position.y = persPtVec[i].y;
        }

        road[0].setXvanish( Xv ); roadVtx[0].position.x = road[0].x + Level::winW/2.0f;
        road[1].setXvanish( Xv ); roadVtx[2].position.x = road[1].x + Level::winW/2.0f;
        for( unsigned int i = 0; i < 4; ++i )
        {
            edgeLine[i].setXvanish( Xv );
            edgeVtx[i].position.x = edgeLine[i].x + persPt::X0; edgeVtx[i].position.y = edgeLine[i].y;
        }
        for( unsigned int i = 0; i < 8; ++i )
        {
            sideWall[i].setXvanish( Xv );
            wallVtx[i].position.x = sideWall[i].x + persPt::X0; wallVtx[i].position.y = sideWall[i].y;
        }
        // back wall
        wallVtx[8].position.x = sideWall[2].x + persPt::X0; wallVtx[8].position.y = sideWall[2].y;// -xMax, yMax
        wallVtx[9].position.x = sideWall[6].x + persPt::X0; wallVtx[9].position.y = sideWall[6].y;// xMax, yMax
        wallVtx[10].position.x = sideWall[7].x + persPt::X0; wallVtx[10].position.y = sideWall[7].y;// xMax, -yMax
        wallVtx[11].position.x = sideWall[3].x + persPt::X0; wallVtx[11].position.y = sideWall[3].y;// -xMax, -yMax

        // floor
        floorCeilingVtx[0].position.x = sideWall[0].x + persPt::X0; floorCeilingVtx[0].position.y = sideWall[0].y;//-xmax, Yg
        floorCeilingVtx[1].position.x = sideWall[3].x + persPt::X0; floorCeilingVtx[1].position.y = sideWall[3].y;//-xmax, Yh +
        floorCeilingVtx[2].position.x = sideWall[7].x + persPt::X0; floorCeilingVtx[2].position.y = sideWall[7].y;// xmax, Yh +
        floorCeilingVtx[3].position.x = sideWall[4].x + persPt::X0; floorCeilingVtx[3].position.y = sideWall[4].y;// xmax, Yg
        // ceiling
        floorCeilingVtx[4].position.x = sideWall[1].x + persPt::X0; floorCeilingVtx[4].position.y = sideWall[1].y;//-xmax, Yg
        floorCeilingVtx[5].position.x = sideWall[2].x + persPt::X0; floorCeilingVtx[5].position.y = sideWall[2].y;//-xmax, Yh +
        floorCeilingVtx[6].position.x = sideWall[6].x + persPt::X0; floorCeilingVtx[6].position.y = sideWall[6].y;// xmax, Yh +
        floorCeilingVtx[7].position.x = sideWall[5].x + persPt::X0; floorCeilingVtx[7].position.y = sideWall[5].y;// xmax, Yg

        horizonLine[0].position.x = sideWall[2].x + persPt::X0; horizonLine[1].position.x = sideWall[6].x + persPt::X0;
        groundLine[0].position.x = sideWall[1].x + persPt::X0; groundLine[1].position.x = sideWall[5].x + persPt::X0;
    }

    // flyBox
    if( flyBox.v.z + moveZ*moveSpeedZ > 0.0f && flyBox.pos.z > persPt::Yg + 1000.0f ) { flyBox.setPosition( flyBox.pos.x, flyBox.pos.y, persPt::Yh + dYh ); }
    if( gravity.y != 0.0f ) flyBox.v += gravity*dt;
    flyBox.update( Xv, dt );
    if( !moveZ ) flyBox.keepInBounds( -xMax, xMax, -yMax, yMax, persPt::Yg, persPt::Yh + dYh );

    return;
}

void lvl_perspect::draw( sf::RenderTarget& RT ) const
{
    RT.draw( horizonLine, 2, sf::Lines );
    RT.draw( vanishPt, 2, sf::Lines );
    RT.draw( wallVtx, 12, sf::Quads );
    RT.draw( floorCeilingVtx, 8, sf::Quads );
    RT.draw( roadVtx, 3, sf::LinesStrip );
    RT.draw( edgeVtx, 4, sf::LinesStrip );

    RT.draw( &vtxVec[0], vtxVec.size(), sf::Lines );
    boxA.draw(RT,viewPos);
    flyBox.draw(RT);
    for( const auto& bx : boxVec ) bx.draw(RT);
    RT.draw( groundLine, 2, sf::Lines );
    return;
}

bool lvl_perspect::makeBoxControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    boxControl.init( hbPos, csPos, sf::Vector2f(300.0f,120.0f), "BOX" );
    boxControl.pButtVec.push_back( &boxMS );

    cloneButt.init( csPos.x + 10.0f, csPos.y + 90.0f, 40.0f, 20.0f, [this](){ boxVec.push_back( boxA ); }, "Clone" );
    boxControl.pButtVec.push_back( &cloneButt );

    std::ifstream fin( "include/levels/perspect/boxMS.txt" );
    if( !fin ) { std::cout << "\n no boxMS data "; return false; }
    sf::Vector2f ofst(10.0f,10.0f);
    boxMS.init( csPos + ofst, &boxMsStrip, fin );
    fin.close();

    if( boxMS.sdataVec.size() != 6 ) { std::cout << "\nbad boxMS data"; return false; }
    stripData* pSD = &boxMS.sdataVec.front();


    pSD[0].pSetFunc =     [this](float x){ boxA.init( x, boxA.pos.y, boxA.pos.z, boxA.dimX, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); boxA.setXvanish( Xv ); };// posX
    pSD[1].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, x, boxA.pos.z, boxA.dimX, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); boxA.setXvanish( Xv ); };// posY
    pSD[2].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, x, boxA.dimX, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); boxA.setXvanish( Xv ); };// posZ
    pSD[3].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, boxA.pos.z, x, boxA.dimY, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); boxA.setXvanish( Xv ); };// dimX
    pSD[4].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, boxA.pos.z, boxA.dimX, x, boxA.dimZ, boxA.vtx[0].color, boxA.drawFaces ); boxA.setXvanish( Xv ); };// dimY
    pSD[5].pSetFunc = [this](float x){ boxA.init( boxA.pos.x, boxA.pos.y, boxA.pos.z, boxA.dimX, boxA.dimY, x, boxA.vtx[0].color, boxA.drawFaces ); boxA.setXvanish( Xv ); };// dimZ
    boxMsStrip.reInit( *pSD );// posX
    boxA.init( pSD[0].xCurr, pSD[1].xCurr, pSD[2].xCurr, pSD[3].xCurr, pSD[4].xCurr, pSD[5].xCurr, boxA.vtx[0].color, boxA.drawFaces );

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

    pSD[0].pSetFunc =     [this](float x){ flyBox.setPosition( x, flyBox.pos.y, flyBox.pos.z ); };// posX
    pSD[1].pSetFunc = [this](float x){ flyBox.setPosition( flyBox.pos.x, x, flyBox.pos.z ); };// posY
    pSD[2].pSetFunc = [this](float x){ flyBox.setPosition( flyBox.pos.x, flyBox.pos.y, x ); };// posZ
    pSD[3].pSetFunc = [this](float x){ flyBox.v.x = x; };// flyVel.x
    pSD[4].pSetFunc = [this](float x){ flyBox.v.y = x; };// flyVel.y
    pSD[5].pSetFunc = [this](float x){ flyBox.v.z = x; };// flyVel.z
    pSD[6].pSetFunc = [this](float x){ gravity.y = x; };// gravity

    flyBox.init( pSD[0].xCurr, pSD[1].xCurr, pSD[2].xCurr, flyBox.dimX, flyBox.dimY, flyBox.dimZ, flyBox.vtx[0].color, flyBox.drawFaces );
    flyBoxMsStrip.reInit( *pSD );// posX

    posZmsg.setFont( *button::pFont );
    posZmsg.setCharacterSize( 12 );
    posZmsg.setColor( sf::Color::White );
    posZmsg.setPosition( csPos.x + 50.0f, csPos.y + 90.0f );
    posZmsg.setString( "pos.z:" );
    posZnumMsg = posZmsg;
    posZnumMsg.setPosition( csPos.x + 114.0f, csPos.y + 30.0f );
    posZnumMsg.setString( "0" );

    flyBoxControl.updateAll = [this](  ){ to_SF_string( posZnumMsg, flyBox.pos.z ); };
    flyBoxControl.drawAll = [this]( sf::RenderTarget& RT ){ RT.draw( posZnumMsg ); };

    return true;
}

bool lvl_perspect::makeEnviroControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    enviroControl.init( hbPos, csPos, sf::Vector2f(300.0f,120.0f), "enviro" );
    enviroControl.pButtVec.push_back( &enviroMS );

    std::ifstream fin( "include/levels/perspect/enviroMS.txt" );
    if( !fin ) { std::cout << "\n no enviroMS data "; return false; }
    sf::Vector2f ofst(10.0f,10.0f);
    enviroMS.init( csPos + ofst, &enviroMsStrip, fin );
    fin.close();

    if( enviroMS.sdataVec.size() != 6 ) { std::cout << "\nbad enviroMS data"; return false; }
    stripData* pSD = &enviroMS.sdataVec.front();
 //   enviroMsStrip.reInit( *pSD );// posX

    std::function<void(void)> pFset = [this]()
    {
        boxA.setPosition( boxA.pos.x, boxA.pos.y, boxA.pos.z );
        boxA.setXvanish( Xv );
        for( auto& bx : boxVec )
        {
            bx.setPosition( bx.pos.x, bx.pos.y, bx.pos.z );
            bx.setXvanish( Xv );
        }
        for( auto& pt : persPtVec ) pt.init( pt.xs, pt.ys, pt.y0 );
        for( unsigned int i = 0; i < persPtVec.size(); ++i )
        {
            persPtVec[i].setXvanish( Xv );
            vtxVec[i].position.x = persPtVec[i].x + Level::winW/2.0f;
            vtxVec[i].position.y = persPtVec[i].y;
        }
        groundLine[0].position.y = groundLine[1].position.y = persPt::Yg;
        horizonLine[0].position.y = horizonLine[1].position.y = persPt::Yh;
        makeBounds();
    };

    pSD[0].pSetFunc = [this,pFset](float x){ persPt::Yg = x; flyFpos.z = persPt::Yg; groundLine[0].position.y = groundLine[1].position.y = persPt::Yg; pFset(); };// Yg
    pSD[1].pSetFunc = [this,pFset](float x){ float dZ = x - persPt::Yh; persPt::Yh = x; boxA.pos.z += dZ; flyBox.pos.z += dZ; for( auto& bx : boxVec ) bx.pos.z += dZ; pFset(); };// Yh
    pSD[2].pSetFunc = [this](float x){ xMax = x; makeBounds(); };// xMax
    pSD[3].pSetFunc = [this](float x){ yMax = x; makeBounds(); };// yMax
    pSD[4].pSetFunc = [this](float x){ dYh = x; makeBounds(); };// dYh
    pSD[5].pSetFunc = [this](float x){ moveSpeedZ = x; };// belt speed
    enviroMsStrip.reInit( *pSD );

    // other
    moveZbutt.init( csPos.x + 30.0f, csPos.y + 86.0f, 50.0f, 24.0f, [this](){ moveZ = moveZbutt.sel ? 1 : 0; }, "Belt" );
    enviroControl.pButtVec.push_back( &moveZbutt );
    moveXLtButt.init( csPos.x + 110.0f, csPos.y + 86.0f, 50.0f, 24.0f, [this](){ moveX = moveXLtButt.sel ? -1 : 0; }, "<--X" ); moveXLtButt.mode = 'I';
    enviroControl.pButtVec.push_back( &moveXLtButt );
    moveXRtButt.init( csPos.x + 170.0f, csPos.y + 86.0f, 50.0f, 24.0f, [this](){ moveX = moveXRtButt.sel ? 1 : 0; }, "X-->" ); moveXRtButt.mode = 'I';
    enviroControl.pButtVec.push_back( &moveXRtButt );

    enviroControl.updateAll = [this](){ if( !( moveXLtButt.sel || moveXRtButt.sel ) ) moveX = 0; };

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
        std::ifstream fin("include/levels/perspect/savedBoxes.txt");
        if( !fin ) { std::cout << "\nno savedBoxes"; return; }
        boxVec.clear();
        persBox tmpBox;
        while( tmpBox.fromFile(fin) ) boxVec.push_back( tmpBox );
    };
    otherControl.pButtVec.push_back( &loadFromFileButt );

    return true;
}
