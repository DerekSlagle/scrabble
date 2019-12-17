#include "lvl_track.h"

void carAdapter::init(  Leg* p_Leg, float Radius, const sf::Text& Tag, sf::Color clr, float vel )
{
    circ.setRadius( Radius );
    circ.setOrigin( Radius, Radius );
    circ.setFillColor( clr );
    tag = Tag;
    vFull = vel;
    LA.init( p_Leg, [this](float x, float y){ circ.setPosition(x,y); tag.setPosition(x,y); }, vel );
}

carAdapter::carAdapter( const carAdapter& rCA ): LA( rCA.LA ), circ(rCA.circ), tag(rCA.tag), pLaneChange(rCA.pLaneChange), tLC(rCA.tLC), tLC_total(rCA.tLC_total), vFull(rCA.vFull)
{
    LA.pSetPosition = [this](float x, float y){ circ.setPosition(x,y); tag.setPosition(x,y); };
}

const Leg* carAdapter::findAdjacentLane( Leg** pPath ) const
{
    const Leg* it1 = LA.pLeg0;// present lane
    const Leg* it20 = LA.pLeg0 == pPath[0] ? pPath[1] : pPath[0];// other lane
    const Leg* it2 = it20;
    while( it1 && it2 && it1 != LA.pLeg )
    {
        it1 = it1->next;
        it2 = it2->next;
        if( !(it1 && it2) || it1 == LA.pLeg0 ) return nullptr;// safegaurd
    }

    return it2;
}

void carAdapter::makeLaneChange( const Leg* p_LaneChange, Leg** pPath )
{
    LA.pLeg0 = LA.pLeg0 == pPath[0] ? pPath[1] : pPath[0];// other lane
    pLaneChange = p_LaneChange;
    tLC = 0.0f;
    sLC = LA.s*pLaneChange->len/LA.pLeg->len;
    waitToPass = false;
}

void carAdapter::update( float dt )
{
    if( pLaneChange )
    {
        tLC += dt;
        if( tLC < tLC_total )// interpolate position
        {
            float x1, y1;// position on 'to' lane
            float vLC = LA.v*pLaneChange->len/LA.pLeg->len;// speed on 'to' lane maintains same relative position, transit to next leg at same time, etc.
            pLaneChange = pLaneChange->update( x1, y1, sLC, vLC, dt );// pLaneChange may change
            LA.pLeg = LA.pLeg->update( LA.x, LA.y, LA.s, LA.v, dt );// pLeg may change
            LA.x += ( tLC/tLC_total )*( x1 - LA.x );// add fraction traveled
            LA.y += ( tLC/tLC_total )*( y1 - LA.y );
        }
        else// complete
        {
            LA.s = sLC;
            LA.pLeg = pLaneChange;
            pLaneChange = nullptr;
            tLC = sLC = 0.0f;
            LA.pLeg = LA.pLeg->update( LA.x, LA.y, LA.s, LA.v, dt );
        }

     //   circ.setPosition( LA.x, LA.y );
     //   tag.setPosition( LA.x, LA.y );
        LA.pSetPosition( LA.x, LA.y );
    }
    else
    {
        if( !waitToPass && LA.v < vFull )
        {
            LA.v += acc;
            if( LA.v > vFull ) LA.v = vFull;
        }

        LA.update(dt);
    }

    return;
}

bool lvl_track::init()
{
    Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(0,20,20);
    button::setHoverBoxColor( Level::clearColor );

    // load the lanes
    std::ifstream inFile("include/levels/track/inLanePath.txt");
    if( !inFile ){ std::cout << "\nnoinLanePath_data"; return false; }
    makePath( inFile, pPath[0] );
    if( !pPath[0] ) return false;
    inFile.close();
    inFile.open("include/levels/track/outLanePath.txt");
    makePath( inFile, pPath[1] );
    if( !pPath[1] ) return false;
    inFile.close();

    // trace paths
    tracePath( *pPath[0], 4.0f, pathVtxVec[0], sf::Color::Blue, sf::Color::Red );
    tracePath( *pPath[1], 4.0f, pathVtxVec[1], sf::Color::Green, sf::Color::Yellow );

    // make cars
    carAdaptVec.resize(2);
    sf::Text label("A", *button::pFont, 14);
    label.setColor( sf::Color::Red );
    label.setOrigin( 0.0f, 12.0f );
    carAdaptVec[0].init( pPath[0], carRadius, label, sf::Color::White, 5.0f );
    label.setString("B");
    carAdaptVec[1].init( pPath[1], carRadius, label, sf::Color::Cyan, 5.7f );
    // from file
    inFile.open("include/levels/track/cars_data.txt");
    if( !inFile ){ std::cout << "\nno cars_data"; return false; }
    label.setColor( sf::Color::White );
    unsigned int laneIdx = 0;
    float s, v;
    char num = '0';
    while( inFile >> laneIdx >> s >> v )
    {
        carAdapter CAtmp;
        label.setString(++num);
        CAtmp.init( pPath[laneIdx], carRadius, label, sf::Color::Blue, v );
        while( s > CAtmp.LA.pLeg->len ){ s -= CAtmp.LA.pLeg->len; CAtmp.LA.pLeg = CAtmp.LA.pLeg->next; }
        CAtmp.LA.s = s;
        CAtmp.LA.pLeg->update( CAtmp.LA.x, CAtmp.LA.y, CAtmp.LA.s, v, 0.0f );// set initial positions
        CAtmp.LA.pSetPosition( CAtmp.LA.x, CAtmp.LA.y );
        carAdaptVec.push_back( CAtmp );
    }

    const float Rind = 3.0f;
    sf::CircleShape indCirc( Rind );
    indCirc.setOrigin( 2.0f*Rind, 2.0f*Rind );
    indCirc.setFillColor( sf::Color::Red );
    wtpIndicatorVec.resize( carAdaptVec.size(), indCirc );
    indCirc.setOrigin( -2.0f*Rind, 2.0f*Rind );
    indCirc.setFillColor( sf::Color::Yellow );
    slowIndicatorVec.resize( carAdaptVec.size(), indCirc );
    unsigned int i = 0;
    for( auto& ca : carAdaptVec )// initial assigns invalidated by copying instances
    {
        ca.LA.pSetPosition = [this,&ca,i](float x, float y)
        {
            ca.circ.setPosition(x,y);
            ca.tag.setPosition(x,y);
            if( ca.waitToPass ) wtpIndicatorVec[i].setPosition(x,y);
            if( ca.vFull > ca.LA.v ) slowIndicatorVec[i].setPosition(x,y);
        };
        ca.LA.pSetPosition( ca.LA.x, ca.LA.y );
        ++i;
    }

    // some ui
    label.setString("GO");
    label.setColor( sf::Color::Black );
    label.setOrigin( 0.0f, 0.0f );
    goButt.init( 20.0f, 10.0f, 60.0f, 30.0f, nullptr, label );
    button::RegisteredButtVec.push_back( &goButt );
    label.setString("deny");
    badLaneButt.init( 20.0f, 50.0f, 60.0f, 30.0f, nullptr, label ); badLaneButt.mode = 'I';
    button::RegisteredButtVec.push_back( &badLaneButt );
    // setup pass condition on cars this=0, slow=1, obstruct=2
    label.setString("test");
    passTestButt.init( 20.0f, 90.0f, 60.0f, 30.0f, nullptr, label ); passTestButt.mode = 'I';
    passTestButt.pHitFunc = [this]()
    {
        std::ifstream fin("include/levels/track/passTest.txt");
        if( !fin ) { std::cout << "\nnopassTest data"; return; }
        int startLeg; fin >> startLeg;// from path begin
        for( unsigned int i = 0; i < 3; ++i )
        {
            int laneIdx, k = startLeg; fin >> laneIdx;
            const Leg* pathIter = pPath[ laneIdx ];
            while( k-- > 0 ) pathIter = pathIter->next;
            carAdaptVec[i].LA.pLeg = pathIter;
            carAdaptVec[i].LA.pLeg0 = pPath[ laneIdx ];
            fin >> carAdaptVec[i].LA.s >> carAdaptVec[i].vFull;
            carAdaptVec[i].LA.v = carAdaptVec[i].vFull;
            carAdaptVec[i].update(0.0f);
        }
    };
    button::RegisteredButtVec.push_back( &passTestButt );

    label.setString("AUTO");
    tryAutoButt.init( 20.0f, 130.0f, 60.0f, 30.0f, nullptr, label );
    button::RegisteredButtVec.push_back( &tryAutoButt );
    tryAutoButt.setSel( true );

    makeTrackControl( sf::Vector2f(100.0f,10.0f), sf::Vector2f(156.0f,10.0f) );
    button::RegisteredButtVec.push_back( &trackControl );

    return true;
}

bool lvl_track::handleEvent( sf::Event& rEvent ){ return true; }

void lvl_track::update( float dt )
{
    if( goButt.sel )
    {
        for( carAdapter& rCA : carAdaptVec )
        {
            if( rCA.pLaneChange ) continue;// already changing lane
            rCA.waitToPass = false;// assign renewed each call while waiting
            for( carAdapter& CA : carAdaptVec )
            {
                if( &CA == &rCA ) continue;// self
                if( rCA.LA.v < CA.LA.v ) continue;// CA is faster

                float sep = -1.0f;// presume none
                const float sepMin = 2.0f*carRadius;
                if( CA.LA.pLeg == rCA.LA.pLeg )// on same Leg
                {
                    if( rCA.LA.s > CA.LA.s ) continue;// CA is behind
                    sep = CA.LA.s - rCA.LA.s;
                    if( sep < sepMin ) rCA.LA.s = CA.LA.s - sepMin;
                }
                else if( CA.LA.pLeg == rCA.LA.pLeg->next )// on next Leg
                {
                    sep = CA.LA.s + rCA.LA.pLeg->len - rCA.LA.s;
                    if( sep < sepMin ) rCA.LA.s = CA.LA.s + rCA.LA.pLeg->len - sepMin;
                }
                else continue;

                if( sep < ( rCA.vFull - CA.LA.v )*rCA.tLC_total + sepMin )
                {
                    const Leg* pLC = rCA.findAdjacentLane( pPath );
                    if( pLC )
                    {
                        if( checkLane( rCA, pLC ) )// check lane for collision hazards
                            rCA.makeLaneChange( pLC, pPath );// if lane is clear
                        else// wait to pass
                        {
                            rCA.waitToPass = true;
                            if( rCA.LA.v > CA.LA.v )// slow down to match lead car speed
                            {
                                rCA.LA.v -= rCA.acc;
                                if( rCA.LA.v < CA.LA.v ) rCA.LA.v = CA.LA.v;// prevent overshoot
                            }
                        }
                    }
                }
            }// end inner for
        }// outer for

        for( auto& CA : carAdaptVec ) CA.update(dt);
    }

    return;
}

/*
void lvl_track::update( float dt )
{
    if( goButt.sel )
    {
        for( carAdapter& rCA : carAdaptVec )
        {
            if( rCA.pLaneChange ) continue;// already changing lane
            const Leg* pLegLook = ( rCA.LA.pLeg->len - rCA.LA.s > rCA.LA.v*rCA.tLC_total ) ? rCA.LA.pLeg : rCA.LA.pLeg->next;
            rCA.waitToPass = false;// assign renewed each call while waiting
            for( carAdapter& CA : carAdaptVec )
            {
                if( &CA == &rCA ) continue;// self
                if( pLegLook != CA.LA.pLeg ) continue;// not on same Leg
                if( rCA.LA.v < CA.LA.v ) continue;// CA is faster
                if( pLegLook == rCA.LA.pLeg && rCA.LA.s > CA.LA.s ) continue;// CA is behind

                float sep = -1.0f;// presume none
                if( pLegLook == rCA.LA.pLeg ) sep = CA.LA.s - rCA.LA.s;
                else sep = CA.LA.s + ( rCA.LA.pLeg->len - rCA.LA.s );
//                if( sep < ( rCA.LA.v - CA.LA.v )*rCA.tLC_total + 2.0f*carRadius )
                if( sep < ( rCA.vFull - CA.LA.v )*rCA.tLC_total + 2.0f*carRadius )
                {
                 //   changeLane( rCA );
                    const Leg* pLC = rCA.findAdjacentLane( pPath );
                    if( pLC )
                    {
                        if( checkLane( rCA, pLC ) )// check lane for collision hazards
                            rCA.makeLaneChange( pLC, pPath );// if lane is clear
                        else// wait to pass
                        {
                            rCA.waitToPass = true;
                            if( rCA.LA.v > CA.LA.v )// slow down to match lead car speed
                            {
                                rCA.LA.v -= rCA.acc;
                                if( rCA.LA.v < CA.LA.v ) rCA.LA.v = CA.LA.v;// prevent overshoot
                            }
                        }
                    }
                }
            }
        }

        for( auto& CA : carAdaptVec ) CA.update(dt);
    }

    return;
}
*/

void lvl_track::draw( sf::RenderTarget& RT ) const
{
    RT.draw( &pathVtxVec[0].front(), pathVtxVec[0].size(), sf::LinesStrip );
    RT.draw( &pathVtxVec[1].front(), pathVtxVec[1].size(), sf::LinesStrip );
    for( const auto& cs : carAdaptVec ) cs.draw(RT);
    for( unsigned int i = 0; i < carAdaptVec.size(); ++i )
    {
        if( carAdaptVec[i].waitToPass ) RT.draw( wtpIndicatorVec[i] );
        if( carAdaptVec[i].vFull > carAdaptVec[i].LA.v ) RT.draw( slowIndicatorVec[i] );
    }

    return;
}

void lvl_track::cleanup()
{
    if( pPath[0] ) std::cout << "\ndeleted " << destroyPath( pPath[0] ) << " legs from path0";
    if( pPath[1] ) std::cout << "\ndeleted " << destroyPath( pPath[1] ) << " legs from path1";
}

bool lvl_track::checkLane( carAdapter& rCA, const Leg* pLC )
{
    if( badLaneButt.sel && &rCA == &carAdaptVec[0] ) return false;
    if( tryAutoButt.sel )
    {
        float LR = pLC->len/rCA.LA.pLeg->len;// use sLC instead
        // conditions for returning false
        for( carAdapter& CA : carAdaptVec )
        {
            if( &CA == &rCA ) continue;// self
        //    if( CA.pLaneChange  ) continue;// is leaving CA.LA.pLeg

            float sep = -1.0f;// presume none
            float sepMin = 0.0f;
            float vRel = 0.0f;

     //       if( pLC == CA.LA.pLeg )// is on same Leg
            if( pLC == CA.LA.pLeg || ( CA.pLaneChange == pLC ) )// is on same Leg
            {
             //   if( rCA.LA.s > CA.LA.s )// CA is behind
                if( rCA.LA.s > CA.LA.s + 2.0f*carRadius )// CA is behind
                {
                 //   vRel = CA.LA.v - rCA.LA.v;
                    vRel = CA.LA.v - rCA.LA.v*LR;
                    if( vRel < 0.0f ) continue;// CA is slower
                //    sep = rCA.LA.s*LR - CA.LA.s;// > 0
                    if( pLC == CA.LA.pLeg ) sep = rCA.LA.s*LR - CA.LA.s;// > 0
                    else sep = rCA.LA.s*LR - CA.sLC;// > 0
                    sepMin = 2.0f*carRadius + vRel*( rCA.tLC_total + vRel/(2.0f*CA.acc) );
                    if( sep < sepMin ) return false;
                }
                else// CA is ahead (or even)
                {
                //    vRel = CA.LA.v - rCA.LA.v;// CA faster if vRel > 0
                    vRel = CA.LA.v - rCA.LA.v*LR;// CA faster if vRel > 0
                    if( pLC == CA.LA.pLeg ) sep = CA.LA.s - rCA.LA.s*LR;// > 0
                    else sep = CA.sLC - rCA.LA.s*LR;// > 0
                    sepMin = 4.0f*carRadius + rCA.tLC_total*( 2.0f*rCA.LA.v - CA.vFull );
                    if( sep < sepMin ) return false;
                }
            }
        //    else  if( pLC->prev == CA.LA.pLeg )// is on prev Leg
            else  if( pLC->prev == CA.LA.pLeg || ( CA.pLaneChange == pLC->prev ) )// is on prev Leg
            {
             //   vRel = CA.LA.v - rCA.LA.v;
                vRel = CA.LA.v - rCA.LA.v*LR;
            //    if( vRel < 0.0f ) continue;// CA is slower
                if( pLC->prev == CA.LA.pLeg ) sep = rCA.LA.s*LR + ( CA.LA.pLeg->len - CA.LA.s );// > 0
                else sep = rCA.LA.s*LR + ( CA.pLaneChange->len - CA.sLC );// > 0
                if( vRel < 0.0f && sep < 2.0f*carRadius ) return false;// CA is slower but too close
                sepMin = 2.0f*carRadius + vRel*( rCA.tLC_total + vRel/(2.0f*CA.acc) );
                if( sep < sepMin ) return false;
            }
         //   else  if( pLC->next == CA.LA.pLeg )// is on next Leg
            else  if( pLC->next == CA.LA.pLeg || ( CA.pLaneChange == pLC->next ) )// is on next Leg
            {
             //   vRel = CA.LA.v - rCA.LA.v;// CA faster if vRel > 0
                vRel = CA.LA.v - rCA.LA.v*LR;// CA faster if vRel > 0
             //   sep = CA.LA.s*LR + ( pLC->len - rCA.LA.s );// > 0
                if( pLC->next == CA.LA.pLeg ) sep = CA.LA.s + ( pLC->len - rCA.LA.s*LR );// > 0
                else sep = CA.sLC + ( pLC->len - rCA.LA.s*LR );// > 0
              //  sepMin = 2.0f*carRadius + rCA.tLC_total*( 2.0f*rCA.LA.v - CA.vFull );
                sepMin = 4.0f*carRadius + rCA.tLC_total*( 2.0f*rCA.LA.v*LR - CA.vFull );
                if( sep < sepMin ) return false;
            }

        }

        return true;
    }

    return true;
}
/*
bool lvl_track::checkLane( carAdapter& rCA, const Leg* pLC )
{
    if( badLaneButt.sel && &rCA == &carAdaptVec[0] ) return false;
    if( tryAutoButt.sel )
    {
        float LR = pLC->len/rCA.LA.pLeg->len;// use sLC instead
        // conditions for returning false
        for( carAdapter& CA : carAdaptVec )
        {
            if( &CA == &rCA ) continue;// self

            float sep = -1.0f;// presume none
            float sepMin = 0.0f;
            float vRel = 0.0f;

            if( pLC == CA.LA.pLeg )// is on same Leg
            {
                if( rCA.LA.s > CA.LA.s )// CA is behind
                {
                    vRel = CA.LA.v - rCA.LA.v;
                    if( vRel < 0.0f ) continue;// CA is slower
                    sep = rCA.LA.s*LR - CA.LA.s;// > 0
                    sepMin = 2.0f*carRadius + vRel*( rCA.tLC_total + vRel/(2.0f*CA.acc) );
                    if( sep < sepMin ) return false;
                }
                else// CA is ahead (or even)
                {
                    vRel = CA.LA.v - rCA.LA.v;// CA faster if vRel > 0
                    sep = CA.LA.s - rCA.LA.s*LR;// > 0
                    sepMin = 2.0f*carRadius + rCA.tLC_total*( 2.0f*rCA.LA.v - CA.vFull );
                    if( sep < sepMin ) return false;
                }
            }
            else  if( pLC->prev == CA.LA.pLeg )// is on prev Leg
            {
                vRel = CA.LA.v - rCA.LA.v;
                if( vRel < 0.0f ) continue;// CA is slower
                sep = rCA.LA.s*LR + ( CA.LA.pLeg->len - CA.LA.s );// > 0
                sepMin = 2.0f*carRadius + vRel*( rCA.tLC_total + vRel/(2.0f*CA.acc) );
                if( sep < sepMin ) return false;
            }
            else  if( pLC->next == CA.LA.pLeg )// is on next Leg
            {
                vRel = CA.LA.v - rCA.LA.v;// CA faster if vRel > 0
                sep = CA.LA.s*LR + ( pLC->len - rCA.LA.s );// > 0
                sepMin = 2.0f*carRadius + rCA.tLC_total*( 2.0f*rCA.LA.v - CA.vFull );
                if( sep < sepMin ) return false;
            }

        }

        return true;
    }

    return true;
}

bool lvl_track::changeLane( carAdapter& rCA )
{
    const Leg* it1 = rCA.LA.pLeg0;// present lane
    const Leg* it20 = rCA.LA.pLeg0 == pPath[0] ? pPath[1] : pPath[0];// other lane
    const Leg* it2 = it20;
    while( it1 && it2 && it1 != rCA.LA.pLeg )
    {
        it1 = it1->next;
        it2 = it2->next;
        if( !(it1 && it2) || it1 == rCA.LA.pLeg0 ) return false;// safegaurd
    }

    rCA.LA.pLeg0 = it20;
    rCA.pLaneChange = it2;
    rCA.tLC = 0.0f;
    rCA.sLC = rCA.LA.s*it2->len/it1->len;
    return true;
}   */

void lvl_track::makeTrackControl( sf::Vector2f hbPos, sf::Vector2f csPos )
{
    trackControl.init( hbPos, csPos, sf::Vector2f(250.0f, 120.0f), "TRACK" );
    trackControl.pButtVec.push_back( &trackMS );

    std::ifstream fin( "include/levels/track/trackMS.txt" );
    if( !fin ) { std::cout << "\n no trackMS data "; return; }
    sf::Vector2f ofst(10.0f,10.0f);
    trackMS.init( csPos + ofst, &trackStrip, fin );
    fin.close();

    if( trackMS.sdataVec.size() != 4 ) { std::cout << "\nbad trackMS data"; return; }
    stripData* pSD = &trackMS.sdataVec.front();
    carAdapter* pCA = &carAdaptVec[0];
    pSD[0].pSetFunc =     [this,pCA](float x){ pCA->LA.v = pCA->vFull = x; };// v1
    pCA->LA.v = pCA->vFull = pSD[0].xCurr;
    pCA = &carAdaptVec[1];
    pSD[1].pSetFunc = [this,pCA](float x){ pCA->LA.v = pCA->vFull = x; };// v2
    pCA->LA.v = pCA->vFull = pSD[1].xCurr;
    pSD[2].pSetFunc = [this](float x){ for( auto& ca : carAdaptVec ) ca.tLC_total = x; };// tLC
    for( auto& ca : carAdaptVec ) ca.tLC_total = pSD[2].xCurr;
    pSD[3].pSetFunc = [this](float x){ for( auto& ca : carAdaptVec ) ca.acc = x; };// acc
    for( auto& ca : carAdaptVec ) ca.acc = pSD[3].xCurr;
    trackStrip.reInit( *pSD );// v1

    sf::Text label("v1", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    label.setString("L1");
    laneButt1.init( csPos.x+10.0f, csPos.y+90.0f, 30.0f, 20.0f, [this](){ carAdaptVec[0].makeLaneChange( carAdaptVec[0].findAdjacentLane( pPath ), pPath ); }, label );
    trackControl.pButtVec.push_back( &laneButt1 );
    label.setString("L2");
    laneButt2.init( csPos.x+70.0f, csPos.y+90.0f, 30.0f, 20.0f, [this](){ carAdaptVec[1].makeLaneChange( carAdaptVec[1].findAdjacentLane( pPath ), pPath ); }, label );
    trackControl.pButtVec.push_back( &laneButt2 );
}
