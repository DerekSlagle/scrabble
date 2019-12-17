#include "lvl_paths.h"

bool lvl_paths::init()
{
   Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,20,20);
    button::setHoverBoxColor( Level::clearColor );
    inertAdaptVec.resize(1);

    sf::Text label("start", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    startButt.init( 50.0f, 20.0f, 60.0f, 30.0f, [this](){  reInit(); anyMoving = true; }, label );
    button::RegisteredButtVec.push_back( &startButt );
    label.setString("HIT");
    hitButt.init( 20.0f, 20.0f, 25.0f, 30.0f, nullptr, label );
    button::RegisteredButtVec.push_back( &hitButt );
    label.setString("brake");
    brakeButt.init( 50.0f, 70.0f, 60.0f, 30.0f, nullptr, label ); brakeButt.mode = 'I';
    button::RegisteredButtVec.push_back( &brakeButt );
    label.setString("+vel");
    accelButt.init( 120.0f, 60.0f, 40.0f, 20.0f, nullptr, label ); accelButt.mode = 'I';
    button::RegisteredButtVec.push_back( &accelButt );
    label.setString("-vel");
    decelButt.init( 120.0f, 85.0f, 40.0f, 20.0f, nullptr, label ); decelButt.mode = 'I';
    button::RegisteredButtVec.push_back( &decelButt );
    label.setString("ALL");
    allOr1Butt.init( 20.0f, 70.0f, 25.0f, 30.0f, nullptr, label );
    allOr1Butt.setSel(true);
    button::RegisteredButtVec.push_back( &allOr1Butt );

    label.setString("path list");
    sf::Vector2f listPos( 50.0f, 120.0f ), buttSz(50.0f,25.0f);
    pathList.init( listPos.x, listPos.y, buttSz.x, buttSz.y, label );
    pathList.persist = 3;
    pathList.ownButts = true;
    button::RegisteredButtVec.push_back( &pathList );
    // sub list for normal paths
    label.setString("normal");
    buttonList* pBL = new buttonList( listPos.x, listPos.y + 1.1f*buttSz.y, buttSz.x, buttSz.y, label );
    pBL->persist = 3;
    pBL->ownButts = true;
    pathList.pButtVec.push_back( pBL );
    std::ifstream inFile("include/levels/paths/normalPathList.txt");
    if( !inFile ){ std::cout << "\nno normalPathList"; return false; }
    std::string fNameStr, labelStr;
    char fType = 'N';// N or F
    int i = 0;
    while( inFile >> fNameStr >> labelStr >> fType )// add a button
    {
        label.setString( labelStr.c_str() );
        buttonRect* pBR = new buttonRect( listPos.x + 1.1f*buttSz.x, listPos.y + 1.1f*(i+1)*buttSz.y, buttSz.x, buttSz.y, [this,fType,fNameStr](){ loadNewPath( fNameStr.c_str(), fType ); }, label );
        pBL->pButtVec.push_back( pBR );
        ++i;
    }
    inFile.close();
    // sub list for function paths
    label.setString("function");
    pBL = new buttonList( listPos.x, listPos.y + 2.2f*buttSz.y, buttSz.x, buttSz.y, label );
    pBL->persist = 3;
    pBL->ownButts = true;
    pathList.pButtVec.push_back( pBL );
    inFile.open("include/levels/paths/functionPathList.txt");
    if( !inFile ){ std::cout << "\nno functionPathList"; return false; }
    i = 0;
    while( inFile >> fNameStr >> labelStr >> fType )// add a button
    {
        label.setString( labelStr.c_str() );
        buttonRect* pBR = new buttonRect( listPos.x + 1.1f*buttSz.x, listPos.y + 1.1f*(i+2)*buttSz.y, buttSz.x, buttSz.y, [this,fType,fNameStr](){ loadNewPath( fNameStr.c_str(), fType ); }, label );
        pBL->pButtVec.push_back( pBR );
        ++i;
    }
    inFile.close();

    // motion
    float rad = 18.0f;
    circ.setRadius( rad );
    circ.setOrigin( rad, rad );
    circ.setFillColor( sf::Color(150,150,150) );
//    if( !ballTxtB.loadFromFile("images/thunderMarble.jpg") ) { std::cout << "no ballTxtA image data\n"; return false; }
 //   circ.setTexture( &ballTxtB );

    inFile.open("include/levels/paths/init_data.txt");
    if( !inFile ){ std::cout << "\nno lvl_paths_data"; return false; }
    std::string pathFileName;
    inFile >> pathFileName;
    std::ifstream pathFile(  pathFileName.c_str() );
    if( !pathFile ) { std::cout << "\nno pathFile"; return false; }
    makePath( pathFile, pPath );
    if( !pPath ) return false;
    pathFile.close();
    inFile >> v1;

    inFile >> pathFileName;
    pathFile.open(  pathFileName.c_str() );
    if( !pathFile ) { std::cout << "\nno pathFile"; return false; }
    makePath( pathFile, pPath_2 );
    if( !pPath ) return false;
    pathFile.close();
    inFile >> v2;

    inFile.close();
    inertAdaptVec[0].init( pPath, [this](float x, float y){ circ.setPosition(x,y); }, v1 );
    legAdapt_2.init( pPath_2, [this](float x, float y){ circ_2.setPosition(x,y); }, v2 );

    // last good
    listPos.x = 250.0f; listPos.y = 20.0f;
    label.setString("controls");
    controlList.init( listPos.x, listPos.y, buttSz.x, buttSz.y, label );
    controlList.persist = 3;
    controlList.ownButts = false;
    button::RegisteredButtVec.push_back( &controlList );

     sf::Vector2f hbPos( listPos.x, listPos.y + 1.1f*buttSz.y ) ,csPos( listPos.x + 1.1f*buttSz.x, listPos.y );
    makeLaunchControl( hbPos, csPos );
    controlList.pButtVec.push_back( &launchControl );
    hbPos.y += 1.1f*buttSz.y;
    makeInertiaControl( hbPos, csPos );
    controlList.pButtVec.push_back( &inertiaControl );
    hbPos.y += 1.1f*buttSz.y;
    makePathControl( hbPos, csPos );// path must exist
    controlList.pButtVec.push_back( &pathControl );
    hbPos.y += 1.1f*buttSz.y;
    makePathControl_2( hbPos, csPos );// path must exist
    controlList.pButtVec.push_back( &pathControl_2 );

    tracePath( *pPath, 4.0f, pathVtxVec, sf::Color::Blue, sf::Color::Red );
    if( !reInit() ) return false;

 //   makePathControl();
 //   button::RegisteredButtVec.push_back( &pathControl );
 //   makeInertiaControl();
 //   button::RegisteredButtVec.push_back( &inertiaControl );

    // colliders
    if( !ballTxtA.loadFromFile("images/soccerBallC.png") ) { std::cout << "no ballTxtA image data\n"; return false; }
    rad = 25.0f;
    sf::CircleShape tmpCrc;
    tmpCrc.setRadius( rad );
    tmpCrc.setOrigin( rad, rad );
    tmpCrc.setFillColor( sf::Color::White );
    tmpCrc.setTexture( &ballTxtA );
    freeCircVec.push_back( tmpCrc );
    inertialAdapter tmpIA;
    sf::CircleShape* pCircle = &freeCircVec.back();
    tmpIA.pSetPosition = [pCircle](float x, float y) { pCircle->setPosition(x,y); };
    tmpIA.R = pCircle->getRadius();
    tmpIA.m = 3.0f;
    tmpIA.setPosition( 50.0f, 200.0f );// initially
    freeAdaptVec.push_back( tmpIA );

    // Irregular polygon
    inFile.open( "include/levels/paths/gfPolyIR.txt" );// i = 0
    if( !inFile ) { std::cout << "no gfPolyIR data\n"; return false; }
    pgIrreg.init(inFile);
    gfPolyIR.tPeriod = -1.0f;
    gfPolyIR.showAxes = false;
    gfPolyIR.gfColor = sf::Color::Red;
    gfPolyIR.fnumPts = [this](void) { return pgIrreg.numPts(); };
    gfPolyIR.fPt_n = [this](size_t i) { return pgIrreg.ptPos(i); };
    gfPolyIR.init( inFile, [this](float& y, float& ypm, float x, float t) { float X = x-t*gfPolyIR.dxdt; pgIrreg.r_rPm(y,ypm,X); }, nullptr );
    inFile.close();

    // regular polygon
    inFile.open( "include/levels/paths/gfPolygonReg.txt" );// i = 0
    if( !inFile ) { std::cout << "no gfPolygonReg data\n"; return false; }
    pgReg.init(inFile);
    gfPolygon.tPeriod = -1.0f;
    gfPolygon.showAxes = false;
    gfPolygon.gfColor = sf::Color::Yellow;
    gfPolygon.fnumPts = [this](void) { return pgReg.numPts(); };
    gfPolygon.fPt_n = [this](size_t i) { return pgReg.ptPos(i); };
    gfPolygon.init( inFile, [this](float& y, float& ypm, float x, float t) { float X = x-t*gfPolygon.dxdt; pgReg.r_rPm(y,ypm,X); }, nullptr );
    inFile.close();

    pGfunc = &gfPolygon;

    hbPos.y += 1.1f*buttSz.y;
    gfCsurf.init( hbPos, csPos, sf::Vector2f(236.0f,150.0f), "regPoly" );
    inFile.open( "include/levels/paths/gfPolygonRegMS.txt" );
    if( !inFile ) { std::cout << "\n no gfPolygonRegMS data " << i; return false; }
    gfMs.init( sf::Vector2f( csPos.x+10.0f, csPos.y+10.0f ), &gfVoh, inFile );
    inFile.close();
    label.setString("direction");
    gfSlider.init( csPos.x+10.0f, csPos.y+100.0f, 160.0f, 20.0f, label, -3.1416f, 3.1416f, atan2f( pGfunc->Tu.y, pGfunc->Tu.x), [this](float x){ pGfunc->setRotation(x); }  );
    gfCsurf.pButtVec.push_back( &gfMs );
    gfCsurf.pButtVec.push_back( &gfSlider );
    controlList.pButtVec.push_back( &gfCsurf );

    stripData* pSD = &gfMs.sdataVec.front();
    if( gfMs.sdataVec.size() >= 3 )
    {
        // number of sides
        pSD->pSetFunc = [this](float x)
        {
            pgReg.setNsides( (int)floor(x) );
            gfPolygon.vtxVec.clear();
            gfPolygon.vtxVec.resize( 1 + pgReg.numPts() );
            for( auto& vtx : gfPolygon.vtxVec ) vtx.color = gfPolygon.gfColor;
            gfPolygon.setGraph();
        };
        gfVoh.reInit( *pSD );
     //   MsVec[3].selButtVec.front().setSel(true);
        ++pSD;// radius
        pSD->pSetFunc = [this](float x){ pgReg.R = x; gfPolygon.setGraph(); };
        pSD->pSetFunc( pSD->xCurr );
        ++pSD;// dxdt
        pSD->pSetFunc = [this](float x) { gfPolygon.set_dxdt(x); };
        pSD->pSetFunc( pSD->xCurr );
    }
    else { std::cout << "\ngfPolygonRegMS: sdataVec.size() < 3"; return false; }


    return true;
}

bool lvl_paths::loadNewPath( const char* fileName, char type )// type = 'N'=normal, 'F'=function
{
    std::ifstream pathFile( fileName );
    if( !pathFile ){ std::cout << "\nno " << fileName << " path data"; return false; }
    destroyPath( pPath );
    if( type == 'N' ) makePath( pathFile, pPath );
    else if( type == 'F' ) makeFuncPath( pathFile, pPath );
    pathVtxVec.clear();
    tracePath( *pPath, 4.0f, pathVtxVec, sf::Color::Blue, sf::Color::Red );

    sf::Vector2f hbPos( pathControl.homeButt.pos.x, pathControl.homeButt.pos.y ) ,csPos( pathControl.bkRect.getPosition().x, pathControl.bkRect.getPosition().y );
    makePathControl( hbPos, csPos );
    reInit();
    anyMoving = false;
    return true;
}

bool lvl_paths::reInit()// from file data
{
    if( !pPath ) return false;

    unsigned int numOnPath = abs( floor( numStrip.fVal ) );
    if( numOnPath > 10 ) { std::cout << "\nerror: numOnPath = " << numOnPath; return false; }

    inertAdaptVec.resize(numOnPath);
    circVec.resize(numOnPath, circ);

    for( unsigned int i = 0; i < numOnPath; ++i )
    {
        inertAdaptVec[i].pLeg0 = pPath;// may have changed
        inertAdaptVec[i].pSetPosition = [this,i](float x, float y){ circVec[i].setPosition(x,y); };
     //   inertAdaptVec[i].offX = 50.0f; inertAdaptVec[i].offY = 100.0f;
        inertAdaptVec[i].reset( v1, i*delayStrip.fVal );
        inertAdaptVec[i].initInertial( Cspring, Cdamp, circVec[i].getRadius(), Mass );
    }

    return true;
}

void lvl_paths::makePathControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    pathControl.pButtVec.clear();
    Leg *pLeg = pPath, *pLegF = pPath;
    sf::Text label("vel0", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    pathControl.init( hbPos, csPos, sf::Vector2f(200.0f, 170.0f), "PATH" );
    pathControl.pButtVec.push_back( &speedStrip );
    pathControl.pButtVec.push_back( &pathMS );
    pathMS.sdataVec.clear();
    pathMS.selButtVec.clear();

    speedStrip.init( csPos.x+10.0f, csPos.y+20.0f, 120.0f, 20.0f, label, -10.0f, 10.0f, v1, &v1 );
    buttonRect tmpRect;
    stripData tmpSD;
    label.setString("ACC");
    float buttW = 36.0f, buttH = 25.0f, spcX = 8.0f;
    sf::Vector2f buttPos( csPos.x+10.0f, csPos.y+70.0f );
    unsigned int legCount = 1;

    if( !pLeg->prev )// is an entry leg
    {
        pLeg->acc = 0.0f;
        tmpSD.init( "accel", -0.1f, 0.1f, 0.0f, [pLeg](float x){ pLeg->acc = x; } );// assigned in lvl_SparkAni
        pathMS.sdataVec.push_back( tmpSD );
        tmpRect.init( buttPos.x, buttPos.y, buttW, buttH, nullptr, label );// ptr requrs * to stripData
        pathMS.selButtVec.push_back( tmpRect );
        buttPos.x += buttW + spcX;
        pLeg = pLegF = pLeg->next;
        ++legCount;
    }


    if( pLeg )// path may not close
    {
        do
        {
            pLeg->acc = 0.0f;
            tmpSD.init( "accel", -0.1f, 0.1f, 0.0f, [pLeg](float x){ pLeg->acc = x; } );// assigned in lvl_SparkAni
            pathMS.sdataVec.push_back( tmpSD );
            tmpRect.init( buttPos.x, buttPos.y, buttW, buttH, nullptr, label );// ptr requrs * to stripData
            pathMS.selButtVec.push_back( tmpRect );
            buttPos.x += buttW + spcX;
            pLeg = pLeg->next;
        }while( pLeg && pLeg != pLegF && legCount++ < 4 );
    }

    if( !pathMS.sdataVec.empty() )
    {
        pathMS.pFS = &accelStrip;
        pathMS.mseOver = false;
        pathMS.sel = true;
        stripData* pSD = &pathMS.sdataVec[0];
        label.setString( pSD->label.c_str() );
        accelStrip.init( csPos.x+10.0f, csPos.y+115.0f, 120.0f, 22.0f, label, pSD->xMin, pSD->xMax, pSD->xCurr, pSD->pSetFunc );
        if( !pathMS.selButtVec.empty() ){ pathMS.idx_curr = 0; pathMS.selButtVec[0].setSel(true); }
    }
}

void lvl_paths::makePathControl_2( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    if( !pPath_2 ) return;
    pathControl_2.pButtVec.clear();
    Leg *pLeg = pPath_2, *pLegF = pPath_2;
    sf::Text label("vel0", *button::pFont, 12);
    label.setColor( sf::Color::Black );

    pathControl_2.init( hbPos, csPos, sf::Vector2f(200.0f, 260.0f), "path2" );
    pathControl_2.pButtVec.push_back( &speedStrip_2 );
    pathControl_2.pButtVec.push_back( &posStrip_2 );
    pathControl_2.pButtVec.push_back( &pathMS_2 );
    pathControl_2.pButtVec.push_back( &useP2Butt );
    pathMS_2.sdataVec.clear();
    pathMS_2.selButtVec.clear();

    speedStrip_2.init( csPos.x+10.0f, csPos.y+20.0f, 120.0f, 20.0f, label, -10.0f, 10.0f, v2, &v2 );
    label.setString("start pos");
    posStrip_2.init( csPos.x+10.0f, csPos.y+70.0f, 120.0f, 20.0f, label, 0.0f, pPath_2->len, 0.0f, nullptr );
    buttonRect tmpRect;
    stripData tmpSD;
    label.setString("ACC");
    float buttW = 36.0f, buttH = 25.0f, spcX = 8.0f;
    sf::Vector2f buttPos( csPos.x+10.0f, csPos.y+120.0f );
    unsigned int legCount = 1;

    if( !pLeg->prev )// is an entry leg
    {
        pLeg->acc = 0.0f;
        tmpSD.init( "accel", -0.1f, 0.1f, 0.0f, [pLeg](float x){ pLeg->acc = x; } );// assigned in lvl_SparkAni
        pathMS_2.sdataVec.push_back( tmpSD );
        tmpRect.init( buttPos.x, buttPos.y, buttW, buttH, nullptr, label );// ptr requrs * to stripData
        pathMS_2.selButtVec.push_back( tmpRect );
        buttPos.x += buttW + spcX;
        pLeg = pLegF = pLeg->next;
        ++legCount;
    }


    if( pLeg )// path may not close
    {
        do
        {
            pLeg->acc = 0.0f;
            tmpSD.init( "accel", -0.1f, 0.1f, 0.0f, [pLeg](float x){ pLeg->acc = x; } );// assigned in lvl_SparkAni
            pathMS_2.sdataVec.push_back( tmpSD );
            tmpRect.init( buttPos.x, buttPos.y, buttW, buttH, nullptr, label );// ptr requrs * to stripData
            pathMS_2.selButtVec.push_back( tmpRect );
            buttPos.x += buttW + spcX;
            pLeg = pLeg->next;
        }while( pLeg && pLeg != pLegF && legCount++ < 4 );
    }

    if( !pathMS_2.sdataVec.empty() )
    {
        pathMS_2.pFS = &accelStrip_2;
        pathMS_2.mseOver = false;
        pathMS_2.sel = true;
        stripData* pSD = &pathMS_2.sdataVec[0];
        label.setString( pSD->label.c_str() );
        accelStrip_2.init( csPos.x+10.0f, csPos.y+165.0f, 120.0f, 22.0f, label, pSD->xMin, pSD->xMax, pSD->xCurr, pSD->pSetFunc );
        if( !pathMS_2.selButtVec.empty() ){ pathMS_2.idx_curr = 0; pathMS_2.selButtVec[0].setSel(true); }
    }

    label.setString("use P2");
    useP2Butt.init( csPos.x+40.0f, csPos.y+210.0f, 60.0f, 30.0f, nullptr, label );
    useP2Butt.pHitFunc = [this](){
        if( useP2Butt.sel )
        {
            for( auto& la : inertAdaptVec ) la.pBaseLA = &legAdapt_2;
            legAdapt_2.v = v2;
            legAdapt_2.s = posStrip_2.fVal;
            legAdapt_2.pLeg = legAdapt_2.pLeg0;
            legAdapt_2.x = legAdapt_2.pLeg->x( legAdapt_2.s );
            legAdapt_2.y = legAdapt_2.pLeg->y( legAdapt_2.s );
        }
        else for( auto& la : inertAdaptVec ) la.pBaseLA = nullptr;
    };

}

void lvl_paths::makeInertiaControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    inertiaControl.init( hbPos, csPos, sf::Vector2f(240.0f,100.0f), "inertia" );
    inertiaControl.pButtVec.push_back( &inertiaMS );

    std::ifstream fin( "include/levels/paths/inertiaMS.txt" );
    if( !fin ) { std::cout << "\n no inertiaMS data "; return; }
    sf::Vector2f ofst(10.0f,10.0f);
    inertiaMS.init( csPos + ofst, &inertiaStrip, fin );
    fin.close();

    if( inertiaMS.sdataVec.size() != 4 ) { std::cout << "\nbad inertiaMS data"; return; }
    stripData* pSD = &inertiaMS.sdataVec.front();
//    inertiaStrip.reInit( *pSD );// spring
    pSD->pSetFunc =     [this](float x){ Cspring = x; for( auto& ila : inertAdaptVec ) ila.Cs = x; };// spring
    Cspring = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ Cdamp = x; for( auto& ila : inertAdaptVec ) ila.Cd = x; };// damp
    Cdamp = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ Mass = x; for( auto& ila : inertAdaptVec ) ila.m = x; };// mass
    Mass = pSD->xCurr;
    (++pSD)->pSetFunc = [this](float x){ gravity.y = x; };// gravity
    gravity.y = pSD->xCurr;
    inertiaStrip.reInit( *pSD );// spring
}

void lvl_paths::makeLaunchControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    sf::Text label("number", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    numStrip.init( csPos.x+10.0f, csPos.y+20.0f, 120.0f, 20.0f, label, 1.0f, 8.0f, 1.0f, nullptr );
    label.setString("delay");
    delayStrip.init( csPos.x+10.0f, csPos.y+70.0f, 120.0f, 20.0f, label, 10.0f, 50.0f, 20.0f, nullptr );
    launchControl.pButtVec.push_back( &numStrip );
    launchControl.pButtVec.push_back( &delayStrip );
    launchControl.init( hbPos, csPos, sf::Vector2f(200.0f, 120.0f), "launch" );
}

bool lvl_paths::handleEvent( sf::Event& rEvent )
{
    return true;
}

void lvl_paths::update( float dt )
{
    inertialAdapter* pIA = freeAdaptVec.empty() ? nullptr : &freeAdaptVec[0];
 //   sf::CircleShape* pCS = freeCircVec.empty() ? nullptr : &freeCircVec[0];

  /*  if( !button:: pButtMse && pIA )
    {
        if( button::clickEvent_Lt() == 1 )
        {
            pIA->setPosition( button::mseX, button::mseY );
            pIA->v1x = pIA->v1y = 0.0f;
        }
        else if( button::clickEvent_Rt() == 1 )
        {
            sf::Vector2f sep( button::mseX - pIA->x1, button::mseY - pIA->y1 );
            pIA->v1x = 0.1f*sep.x; // 10 px/fr at 100 px distance
            pIA->v1y = 0.1f*sep.y;
        }
    }   */

    if( pIA )
    {
        if( button:: pButtMse ) pIA->update( dt, 0.0f, gravity.y );
        else// mse not over a button
        {
            if( button::clickEvent_Lt() == 1 )
            {
                pIA->setPosition( button::mseX, button::mseY );
                pIA->v1x = pIA->v1y = 0.0f;
            }
            else if( button::clickEvent_Lt() == -1 )// release
            {
                sf::Vector2f sep( button::mseX - pIA->x1, button::mseY - pIA->y1 );
                pIA->v1x = 0.1f*sep.x; // 10 px/fr at 100 px distance
                pIA->v1y = 0.1f*sep.y;
            }
            else if( !button::mseDnLt ) pIA->update( dt, 0.0f, gravity.y );
        }
    }

//    gfPolyIR.drag(false);
    if( pGfunc )
    {
        pGfunc->drag(false);
        pGfunc->update(dt);
    }

    float boxLt = 0.0f, boxTop = 0.0f;
 //   if( button::mseDnRt && !button:: pButtMse ) { boxLt = button::mseX; boxTop = button::mseY; }

//    if( !anyMoving ) return;
    anyMoving = false;

    if( useP2Butt.sel && pPath_2 ) legAdapt_2.update(dt);

  //  for( auto& la : freeAdaptVec )
  //  {
  //      la.update( dt, 0.0f, gravity.y );
  //  }

    for( auto& la : inertAdaptVec )
    {
    //    const Leg* pL = la.pLeg;
    //    la.update(dt);
        la.update( dt, 0.0f, gravity.y );
        if( la.pLeg ) anyMoving = true;
        if( hitButt.sel )
        {
         //   la.hitRigid( circ.getPosition().x, circ.getPosition().y, circ.getRadius() );
         //   la.keepInBox( boxLt, Level::winW, boxTop, Level::winH );
            if( pIA )
            {
             //   la.hitRigid( pCS->getPosition().x, pCS->getPosition().y, pCS->getRadius() );
                if( la.hitFree( pIA->x1, pIA->y1, pIA->v1x, pIA->v1y, pIA->R, pIA->m ) ) pIA->setPosition( pIA->x1, pIA->y1 );
            }
            sf::Vector2f Pos(la.x1, la.y1), vel(la.v1x, la.v1y);
            if( pGfunc && pGfunc->hit( Pos, vel, 0.8f, dt, 0.0f ) )
            {
                la.setPosition( Pos.x, Pos.y );
                la.v1x = vel.x; la.v1y = vel.y;
            }
            la.keepInBox( boxLt, Level::winW, boxTop, Level::winH );
        }
//        if( pL != la.pLeg ) std::cout << "\nv = " << legAdaptVec[0].v;
   //     if( pL != la.pLeg ) std::cout << "\nv = " << inertAdaptVec[0].v;
    }

     if( hitButt.sel && pIA )
     {
        sf::Vector2f Pos(pIA->x1, pIA->y1), vel(pIA->v1x, pIA->v1y);
        if( pGfunc && pGfunc->hit( Pos, vel, 0.8f, dt, 0.0f ) )
        {
            pIA->setPosition( Pos.x, Pos.y );
            pIA->v1x = vel.x; pIA->v1y = vel.y;
        }
        pIA->keepInBox( boxLt, Level::winW, boxTop, Level::winH );
     }


    if( brakeButt.sel )
    {
       if( allOr1Butt.sel ) for( auto& la : inertAdaptVec ){ if( la.delay <= 0.0f && la.pLeg )la.applyBrake( 0.5f ); }
       else inertAdaptVec[0].applyBrake( 0.5f );
    }
    else if( accelButt.sel )
    {
        if( allOr1Butt.sel ) for( auto& la : inertAdaptVec ){ if( la.delay <= 0.0f && la.pLeg )la.v += 0.2f; }
        else inertAdaptVec[0].v += 0.2f;
    }
    else if( decelButt.sel )
    {
        if( allOr1Butt.sel ) for( auto& la : inertAdaptVec ){ if( la.delay <= 0.0f && la.pLeg )la.v -= 0.2f; }
        else inertAdaptVec[0].v -= 0.2f;
    }

 //   if( !anyMoving ) std::cout << "\nv = " << legAdaptVec[0].v;
    return;
}

void lvl_paths::draw( sf::RenderTarget& RT ) const
{
//    RT.draw( circ );
    for( const auto& crcl : circVec ) RT.draw( crcl );
    for( const auto& crcl : freeCircVec ) RT.draw( crcl );
    if( pPath ) RT.draw( &pathVtxVec[0], pathVtxVec.size(), sf::LinesStrip );
    if( pGfunc ) pGfunc->draw(RT);
    return;
}

void lvl_paths::cleanup()
{
    if( pPath )
    {
        std::cout << "\ndeleted " << destroyPath( pPath ) << " legs from path";
    }
    return;
}
