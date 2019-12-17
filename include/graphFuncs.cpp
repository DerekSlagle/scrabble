#include "graphFuncs.h"

// some constants
const float myPI = 3.1415927f;
const float myPIx2 = 2.0f*myPI;

// static members
float graphFuncs::hitOffset = 0.2f;

float DotProd( sf::Vector2f A, sf::Vector2f B ) { return A.x*B.x + A.y*B.y; }
float Length( sf::Vector2f A ) { return sqrtf( DotProd(A,A) ); }

void graphFuncs::init( std::istream& is, std::function<void(float&,float&,float,float)> Fy_yPm, std::function<float(float,float)> Fdydt )
{
    fy_yPm = Fy_yPm;
    fdydt = Fdydt;
    fy = fyPm = nullptr;
    tElap = 0.0f;
    init(is);
}

void graphFuncs::init( std::istream& is, std::function<float(float,float)> Fy , std::function<float(float,float)> FyPm, std::function<float(float,float)> Fdydt )
{
    fy = Fy;
    fyPm = FyPm;
    fdydt = Fdydt;
    fy_yPm = nullptr;
    tElap = 0.0f;
    init(is);
}


void graphFuncs::init( std::istream& is )
{
    is >> coordType >> pos.x >> pos.y;// >> length >> sclY;
    is >> xLt >> xRt >> x0;// x0 = expansion center
    float angle; is >> hitSide;
    // streaming complete
    angle = 0.0f;
    sclX = sclY = 1.0f;
    length = xRt - xLt;

    Tu.x = cosf(angle); Tu.y = sinf(angle);
    Nu.x = -Tu.y; Nu.y = Tu.x;

    // assign vertex colors
    if( xLt < xRt && x0 >= xLt && x0 <= xRt )// all should be good
    {
        dxVtx = 3.0f;
        size_t Nvtx = (size_t)( (xRt-xLt)*sclX/dxVtx );
        if( coordType == 'P' )
        {
            float y = 0.0f, yp = 0.0f;
            if( fy_yPm ) { fy_yPm( yMin, yp, xLt-x0, 0.0f ); yMax = yMin; }
            else if( fy ) { yMin = fy(xLt-x0,0.0f); yMax = yMin; }

            for( float x = xLt; x <= xRt; x += length/100.0f )
            {
                if( fy_yPm ) fy_yPm( y, yp, x-x0, 0.0f );
                else if( fy ) y = fy(x-x0,0.0f);
                if( y < yMin ) yMin = y;
                else if( y > yMax ) yMax = y;
            }
            Nvtx = yMax*length/dxVtx;
        }

        if( fnumPts && fPt_n ) Nvtx = 1 + fnumPts();// graph as discrete points in LinesStrip

        if( Nvtx < 2 ) return;// min for a LinesStrip
        std::cout << "\ngf::init Nvtx = " << Nvtx;
        vtxVec.resize( Nvtx );
        dxVtx = ((xRt-xLt))/(float)( vtxVec.size() - 1 );
        // color
        for( auto& vtx : vtxVec ) vtx.color = gfColor;
        for( size_t i=0; i<6; ++i ) xAxis[i].color = yAxis[i].color = sf::Color::White;
        sf::Text* pTxtArr[] = { &xAxisLabel, &yAxisLabel };
        for( auto pTxt : pTxtArr )
        {
            pTxt->setFont( *button::pFont );
            pTxt->setCharacterSize(20);
            pTxt->setColor( sf::Color::White );
            pTxt->setStyle( sf::Text::Italic );// | sf::Text::Bold );
            std::cout << " ?";
        }
        xAxisLabel.setString("x");
        yAxisLabel.setString("y");
        setPosition( pos );
    }
}

void graphFuncs::setLength( float L )// for coordType == 'C' only
{
    if( coordType != 'C' ) return;

    length = L;
    xRt = L + xLt;
    if( x0 > xRt ) x0 = xRt;
    size_t Nvtx = (size_t)( (xRt-xLt)/dxVtx );
    vtxVec.resize( Nvtx );
    dxVtx = ((xRt-xLt))/(float)( vtxVec.size() - 1 );
    for( auto& vtx : vtxVec ) vtx.color = gfColor;
}

void graphFuncs::draw( sf::RenderTarget& RT ) const
{
    if( showAxes )
    {
        RT.draw( xAxis, 6, sf::Lines );
        RT.draw( yAxis, 6, sf::Lines );
        RT.draw( xAxisLabel );
        RT.draw( yAxisLabel );
    }
    RT.draw( &(vtxVec[0]), vtxVec.size(), sf::LinesStrip );
}

void graphFuncs::setPosition( sf::Vector2f Pos )
{
    pos = Pos;
    float x = xLt;
    float y = 0.0f, yp;// as ref params in f()

//    if( fy_yPm ) { fy_yPm( yMin, yp, xLt-x0 ); yMin *= -1.0f; yMax = yMin; }
 //   else if( fy ) { yMin = fy(xLt-x0); yMin *= -1.0f; yMax = yMin; }

    if( fnumPts && fPt_n )// treated as polar
    {
        yMin = yMax = 0.0f;// at center
        for( size_t i = 0; i < fnumPts(); ++i )
        {
            sf::Vector2f ptPos = fPt_n(i);
            float ang = atan2f( ptPos.y, ptPos.x ) + tElap*dxdt - x0;
            float len = sqrtf( ptPos.x*ptPos.x + ptPos.y*ptPos.y );
            vtxVec[i].position = pos + len*cosf(ang)*Tu + len*sinf(ang)*Nu;
            if( len > yMax ) yMax = len;
        }
        vtxVec.back() = vtxVec.front();
    }
    else
    {
        if( fy_yPm ) { fy_yPm( yMin, yp, xLt-x0, tElap ); yMax = yMin; }
        else if( fy ) { yMin = fy(xLt-x0,tElap); yMax = yMin; }

        for( auto& vtx : vtxVec )
        {
            if( fy_yPm ) fy_yPm( y, yp, x-x0, tElap );
            else if( fy && fyPm ) { y = fy(x-x0,tElap); yp = fyPm(x-x0,tElap); }
            if( coordType == 'C' ) vtx.position = pos - x0*Tu + (x - xLt)*sclX*Tu + y*sclY*Nu;// inverted y axis
            else if( coordType == 'P' ) vtx.position = pos + y*cosf(x-x0)*Tu + y*sinf(x-x0)*Nu;// inverted y axis
         //   else if( coordType == 'P' ) vtx.position = pos + y*cosf(x)*Tu + y*sinf(x)*Nu;// inverted y axis
            else break;
            if( y < yMin ) yMin = y;
            else if( y > yMax ) yMax = y;
            x += dxVtx;
        }
    }

    // the axes
    float tip_dx = 10.0f;
    float tip_dy = 4.0f;
//    xAxis[0].position = pos;
    if( coordType == 'C' )
    {
        xAxis[0].position = pos - x0*Tu;
        xAxis[1].position = pos + (xRt-x0)*Tu;
     //   xAxis[1].position = pos + length*Tu;
        yAxis[0].position = pos - x0*Tu + (x0-xLt)*sclX*Tu - (length/5.0f)*Nu;
        yAxis[1].position = yAxis[0].position + (length/2.0f)*Nu;
    }
    else if( coordType == 'P' )
    {
        xAxis[0].position = pos - yMax*Tu;
        xAxis[1].position = pos + yMax*Tu;
        yAxis[0].position = pos - yMax*Nu;
        yAxis[1].position = pos + yMax*Nu;
    }

    xAxisLabel.setPosition( xAxis[1].position - 2.0f*tip_dx*Tu );// + tip_dx*Nu );
    yAxisLabel.setPosition( yAxis[1].position - 3.0f*tip_dx*Nu + tip_dx*Tu );
    float Ang = 180.0f*atan2f( Tu.y, Tu.x )/3.14159f;
    xAxisLabel.setRotation( Ang );
    yAxisLabel.setRotation( Ang );

 //   xAxis[0].position = pos - x0*Tu;
 //   xAxis[1].position = pos + (xRt-x0)*Tu;
    xAxis[2].position = xAxis[3].position = xAxis[4].position = xAxis[5].position = xAxis[1].position;// tip
    xAxis[3].position += tip_dy*Nu -tip_dx*Tu;
    xAxis[5].position -= tip_dy*Nu +tip_dx*Tu;

    // y axis
 //   yAxis[0].position = pos - x0*Tu + (x0-xLt)*sclX*Tu - (length/5.0f)*Nu;
 //   yAxis[1].position = yAxis[0].position + (length/2.0f)*Nu;
    yAxis[2].position = yAxis[3].position = yAxis[4].position = yAxis[5].position = yAxis[1].position;// tip
    yAxis[3].position += -tip_dy*Tu - tip_dx*Nu;
    yAxis[5].position += tip_dy*Tu - tip_dx*Nu;
}

void graphFuncs::setRotation( float angle )
{
    Tu.x = cosf(angle); Tu.y = sinf(angle);
    Nu.x = -Tu.y; Nu.y = Tu.x;
    setPosition(pos);
}

void graphFuncs::update( float dt )
{
    if( !( fdydt || dxdt ) ) return;// no time dep
  //  if( tPeriod > 0.0f ) tElap += dt;
    tElap += dt;
    if( ( tPeriod > 0.0f && tElap > tPeriod ) || ( tPeriod < 0.0f && tElap < tPeriod ) ) tElap -= tPeriod;
    setPosition(pos);
}

bool graphFuncs::drag( bool wLeft )
{
    static sf::Vector2f grabOfst;
    sf::Vector2f mse( button::msePos() );
    int eventID = wLeft ? button::clickEvent_Lt() : button::clickEvent_Rt();
    bool mseDn = wLeft ? button::mseDnLt : button::mseDnRt;
    if( eventID == 1 )// grab
    {
        const float bd = 50.0f;
        if( (mse.x > (pos.x-bd)) && (mse.x < (pos.x+bd)) && (mse.y > (pos.y-bd)) && (mse.y < (pos.y+bd)) )
        { held = true; grabOfst = pos - mse; }
    }
    else if( held )
    {
        if( mseDn ) setPosition( mse + grabOfst );// drag
        else { setPosition( mse + grabOfst ); held = false; }// release
    }

    return held;
}

void graphFuncs::set_dxdt( float dxDt )// cycle time
{
 //   std::cerr << "\ngf::set_dxdt()";
 //   return;

    if( dxDt != 0.0f )
    {
        tElap *= dxdt/dxDt;
    }
    else
    {
     //   x0 += tElap*dxdt;
        tElap = 0.0f;
     //   if( x0 < xLt ) x0 += length;
     //   else if( x0 > xRt ) x0 -= length;
    }
    dxdt = dxDt; tPeriod = ( dxDt == 0.0f ? -1.0f : length/dxDt );
    if( tPeriod > 0.0f ) while( tElap > tPeriod ) tElap -= tPeriod;
    setPosition( pos );
}

sf::Vector2f graphFuncs::getNorm_xyYpm( float x, float y, float ypm ) const
{
    return fromBase( getNorm_xyYpm_Local( x, y, ypm ) );
}

sf::Vector2f graphFuncs::getNorm_xyYpm_Local( float x, float y, float ypm ) const
{
    sf::Vector2f Ncurve;
    if( coordType == 'C' )
    {
        Ncurve.x = -ypm; Ncurve.y = 1.0f;
        Ncurve /= sqrtf( 1.0f + ypm*ypm );
    }
    else if( coordType == 'P' )
    {
        Ncurve.x = cosf(x);
        Ncurve.y = sinf(x);
        Ncurve -= ( ypm/y )*sf::Vector2f( -Ncurve.y, Ncurve.x );
        Ncurve /= sqrtf( DotProd(Ncurve,Ncurve) );
    }
    return Ncurve;
}

sf::Vector2f graphFuncs::getNorm_xt( float x, float t ) const
{
    float y = 0.0f, slope = 0.0f;
    if( fy && fyPm ) { y = fy(x,t); slope = fyPm(x,t); }
    else if( fy_yPm ) fy_yPm( y, slope, x, t );

    return getNorm_xyYpm( x, y, slope );
}

sf::Vector2f graphFuncs::getPosition_xt( float x, float t ) const
{
    float y = 0.0f, slope = 0.0f;
    if( fy ) { y = fy(x,t); }
    else if( fy_yPm ) fy_yPm( y, slope, x, t );
    return getPosition_xy( x, y );
}

sf::Vector2f graphFuncs::getPosition_xy( float x, float y ) const
{
    sf::Vector2f pt(x,y);
    if( coordType == 'P' )
    {
        pt.x = y*cosf(x);
        pt.y = y*sinf(x);
    }
    return fromBase( pt ) + pos;
}

bool graphFuncs::getGlobalMotion( float xParam, sf::Vector2f& ptPos, sf::Vector2f& ptVel, sf::Vector2f& Ncurve ) const
{
    if( xParam < xLt || xParam > xRt ) return false;// outside of domain
    xParam -= x0;
    float y = 0.0f, slope = 0.0f;
    if( fy && fyPm ) { y = fy(xParam,tElap); slope = fyPm(xParam,tElap); }
    else if( fy_yPm ) fy_yPm( y, slope, xParam, tElap );
    else return false;

    Ncurve = getNorm_xyYpm( xParam, y, slope );

    if( coordType == 'C' )
    {
        ptVel.x = dxdt;
        if( fdydt ) ptVel.y = fdydt( xParam, tElap );
        else ptVel.y = 0.0f;

     //   ptVel = dxdt*( Tu + slope*Nu );
     //   if( fdydt ) ptVel += fdydt( xParam, tElap )*Nu;

        ptVel += DotProd(ptVel,Ncurve)*Ncurve;// both sides
    }
    else if( coordType == 'P' )
    {
   //     ptVel.x = 0.0f;
   //     ptVel.y = 0.0f;
        sf::Vector2f ru( cosf(xParam), sinf(xParam) );
        sf::Vector2f au( -ru.y, ru.x );// orig
        ptVel = dxdt*( slope*ru + y*au );
        if( fdydt ) ptVel += fdydt( xParam, tElap )*ru;
        ptVel += DotProd(ptVel,Ncurve)*Ncurve;// both sides
    }

    ptVel = fromBase( ptVel );
    ptPos = getPosition_xy( xParam, y );// + hitOffset*Ncurve;

    return true;
}

/*
bool graphFuncs::getGlobalMotion( float xParam, sf::Vector2f& ptPos, sf::Vector2f& ptVel, sf::Vector2f& Ncurve ) const
{
    if( xParam < xLt || xParam > xRt ) return false;// outside of domain
    xParam -= x0;
    float y = 0.0f, slope = 0.0f;
    if( fy && fyPm ) { y = fy(xParam,tElap); slope = fyPm(xParam,tElap); }
    else if( fy_yPm ) fy_yPm( y, slope, xParam, tElap );
    else return false;

    if( coordType == 'C' )
    {
    //    ptPos.x = xParam;
    //    ptPos.y = y;
        ptVel.x = dxdt;
        ptVel.y = slope*dxdt;
        if( fdydt ) ptVel.y += fdydt( xParam, tElap );// new

    //    Ncurve.x = -slope; Ncurve.y = 1.0f;
    //    Ncurve /= sqrtf( 1.0f + slope*slope );
    }
    else if( coordType == 'P' )
    {
    //    ptPos.x = y*cosf(xParam);
    //    ptPos.y = y*sinf(xParam);
        ptVel.x = 0.0f;
        ptVel.y = 0.0f;
     //   if( fdydt ) ptVel.y += fdydt( xParam, tElap );// new

     //   Ncurve.x = cosf(xParam);
     //   Ncurve.y = sinf(xParam);
     //   Ncurve -= ( slope/y )*sf::Vector2f( -Ncurve.y, +Ncurve.x );
     //   Ncurve /= sqrtf( DotProd(Ncurve,Ncurve) );
    }

 //   Ncurve = fromBase( Ncurve );
    Ncurve = getNorm_xyYpm( xParam, y, slope );
    ptVel = fromBase( ptVel );
    ptPos = getPosition_xy( xParam, y );
 //   ptPos = fromBase( ptPos ) + pos;


    return true;
}
*/

bool graphFuncs::hit( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff ) const
{
    if( coordType == 'P' ) return hitPolar( Pos, vel, cr, dt );

    sf::Vector2f Pf = Pos - pos;
    Pf = toBase(Pf);
    sf::Vector2f Vel = toBase(vel);
    sf::Vector2f stdOffV = standOff*( Vel.y > 0.0f ? Nu : -Nu );
    Pf += stdOffV;

    sf::Vector2f P0 = Pf - Vel*dt; if( P0.x < -x0 && Pf.x < -x0 ) return false;//{ std::cout << " missLt"; return false; }// miss left
    float L = (xRt-x0)*sclX; if( P0.x > L && Pf.x > L ) return false;// miss right

    if( P0.y < yMin && Pf.y < yMin ) return false;// miss high
    if( P0.y > yMax && Pf.y > yMax ) return false;// miss low

    float Y, slope;// find y, ypm at P0
    if( fy && fyPm ) { Y = fy(P0.x,tElap-dt); slope = fyPm(P0.x,tElap-dt); }
    else if( fy_yPm ) fy_yPm( Y, slope, P0.x, tElap-dt );
    else return false;
    float a = Y - P0.y;// distance above graph at P0
 //   a -= stdOffV.x*Nu.x + stdOffV.y*Nu.y;
    if( fdydt ) Vel.y -= fdydt( Pf.x, tElap );// new
    if( dxdt != 0.0f ) Vel.x -= dxdt;

    if( a < 0.0f && Vel.y > Vel.x*slope )  return false; // below and going down
    if( a > 0.0f && Vel.y < Vel.x*slope ) return false;// above and going up
    // find y, ypm at Pf
    if( fy && fyPm ) Y = fy(Pf.x,tElap);
    else if( fy_yPm ) fy_yPm( Y, slope, Pf.x, tElap );

    float b = Pf.y - Y;// distance below graph at Pf
 //   b += stdOffV.x*Nu.x + stdOffV.y*Nu.y;
    if( a*b < 0.0f ) return false;// didn't cross

    // handle hit
    sf::Vector2f Ph;
    Ph.x = (a*Pf.x + b*P0.x)/( a + b );
    if( fy && fyPm ) { Ph.y = fy(Ph.x,tElap); slope = fyPm(Ph.x,tElap); }
    else if( fy_yPm ) fy_yPm( Ph.y, slope, Ph.x, tElap );

    sf::Vector2f N(slope,-1.0f);// was - Nu
    N /= sqrtf( 1.0f + slope*slope );// normalize
    if( a < 0.0f ) N *= -1.0f;// face incident direction
    Pos = fromBase( Ph + hitOffset*N ) + pos;
    Pos -= stdOffV;

    // xcode
    if( DotProd(Vel,N) < 0.0f ) Vel -= (1.0f + cr)*DotProd(Vel,N)*N;
    if( fdydt ) Vel.y += fdydt( Pf.x, tElap );// new
    if( dxdt != 0.0f ) Vel.x += dxdt;
    vel = fromBase(Vel);

    return true;
}

/*  Last version
bool graphFuncs::hit( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff ) const
{
    if( coordType == 'P' ) return hitPolar( Pos, vel, cr, dt );

    sf::Vector2f Pf = Pos - pos;
    Pf = toBase(Pf);
    sf::Vector2f Vel = toBase(vel);
    sf::Vector2f stdOffV = standOff*( Vel.y > 0.0f ? Nu : -Nu );
    Pf += stdOffV;

    sf::Vector2f P0 = Pf - Vel*dt; if( P0.x < -x0 && Pf.x < -x0 ) return false;//{ std::cout << " missLt"; return false; }// miss left
    float L = (xRt-x0)*sclX; if( P0.x > L && Pf.x > L ) return false;// miss right

    if( P0.y < yMin && Pf.y < yMin ) return false;// miss high
    if( P0.y > yMax && Pf.y > yMax ) return false;// miss low

    float Y, slope;// find y, ypm at P0
    if( fy && fyPm ) { Y = fy(P0.x,tElap-dt); slope = fyPm(P0.x,tElap-dt); }
    else if( fy_yPm ) fy_yPm( Y, slope, P0.x, tElap-dt );
    else return false;
    float a = Y - P0.y;// distance above graph at P0
 //   a -= stdOffV.x*Nu.x + stdOffV.y*Nu.y;
    if( fdydt ) Vel.y -= fdydt( Pf.x, tElap );// new
    if( dxdt != 0.0f )
    {
        Vel.x -= dxdt;// new
        Vel.y += dxdt*slope;// really new
    }

    if( a < 0.0f && Vel.y > Vel.x*slope )  return false; // below and going down
    if( a > 0.0f && Vel.y < Vel.x*slope ) return false;// above and going up
    // find y, ypm at Pf
    if( fy && fyPm ) Y = fy(Pf.x,tElap);
    else if( fy_yPm ) fy_yPm( Y, slope, Pf.x, tElap );

    float b = Pf.y - Y;// distance below graph at Pf
 //   b += stdOffV.x*Nu.x + stdOffV.y*Nu.y;
    if( a*b < 0.0f ) return false;// didn't cross

    // handle hit
    sf::Vector2f Ph;
    Ph.x = (a*Pf.x + b*P0.x)/( a + b );
    if( fy && fyPm ) { Ph.y = fy(Ph.x,tElap); slope = fyPm(Ph.x,tElap); }
    else if( fy_yPm ) fy_yPm( Ph.y, slope, Ph.x, tElap );

 //   if( fy && fyPm ) { Ph.y = fy(Ph.x,tElap-dt/2.0f); slope = fyPm(Ph.x,tElap-dt/2.0f); }
 //   else if( fy_yPm ) fy_yPm( Ph.y, slope, Ph.x, tElap-dt/2.0f );

    sf::Vector2f N(slope,-1.0f);// was - Nu
    N /= sqrtf( 1.0f + slope*slope );// normalize
 //   if( Vel.y < Vel.x*slope ) N *= -1.0f;// face incident direction
    if( a < 0.0f ) N *= -1.0f;// face incident direction
 //   if( a < 0.0f || b < 0.0f ) N *= -1.0f;// face incident direction
    Pos = fromBase( Ph + hitOffset*N ) + pos;
    Pos -= stdOffV;

    // xcode
    sf::Vector2f Vboost;
    Vboost.x = dxdt;
    if( fdydt ) Vboost.y = fdydt( Ph.x, tElap );
    Vel -= Vboost;
    if( DotProd(Vel,N) < 0.0f ) Vel -= (1.0f + cr)*DotProd(Vel,N)*N;
    Vel += Vboost;
    vel = fromBase(Vel);

    return true;
}
*/

bool graphFuncs::hitPolar( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff ) const
{
    if( hitSide == 1 ) return hitPolar_outOnly( Pos, vel, cr, dt, standOff );

    sf::Vector2f Pf = Pos - pos;// position found now
    Pf = toBase(Pf);
    sf::Vector2f Vel = toBase(vel);
  //  sf::Vector2f stdOffV = standOff*( Vel.y > 0.0f ? Nu : -Nu );
  //  Pf += stdOffV;

    sf::Vector2f P0 = Pf - Vel*dt;// position last frame
    float Rf = Length(Pf), R0 = Length(P0);
    if( (Rf < yMin && R0 < yMin) || (Rf > yMax && R0 > yMax) ) return false;//{ std::cout << " missLt"; return false; }// miss left

    float Y, slope;// find y, ypm at P0
    float ang0 = atan2f( P0.y, P0.x );// + x0;
    if( fy && fyPm ) { Y = fy(ang0,tElap-dt); slope = fyPm(ang0,tElap-dt); }
    else if( fy_yPm ) fy_yPm( Y, slope, ang0, tElap-dt );
    else return false;
    float a = Y - R0;// distance above graph at P0
    // find y, ypm at Pf
    float angF = atan2f( Pf.y, Pf.x );// + x0;
    if( fy && fyPm ) Y = fy(angF,tElap);
    else if( fy_yPm ) fy_yPm( Y, slope, angF, tElap );

    float b = Rf - Y;// distance below graph at Pf
    if( a*b < 0.0f ) return false;// didn't cross
    // handle hit
    sf::Vector2f Ph = P0 + ( Pf - P0 )*a/( a + b );// interpolated position where particle crossed the curve
    float angH = atan2f( Ph.y, Ph.x );// + x0;
    if( fy && fyPm ) { Y = fy(angH,tElap); slope = fyPm(angH,tElap); }
    else if( fy_yPm ) fy_yPm( Y, slope, angH, tElap );

    sf::Vector2f ru( cosf(angH), sinf(angH) );
    Ph = Y*ru;

    sf::Vector2f N = getNorm_xyYpm_Local( angH, Y, slope );
//    sf::Vector2f N = getNorm_xyYpm( angH, Y, slope );
    if( a > 0.0f ) N *= -1.0f;// face incident direction
 //   Pos = fromBase(Ph) + pos + hitOffset*N;
    Pos = fromBase( Ph + hitOffset*N ) + pos;
 //   Pos -= stdOffV

    sf::Vector2f au( -ru.y, ru.x );// orig
    sf::Vector2f Vboost = dxdt*Y*au;
    if( fdydt ) Vboost += fdydt( angH, tElap )*ru;

    Vel -= Vboost;
    if( DotProd(Vel,N) < 0.0f ) Vel -= (1.0f + cr)*DotProd(Vel,N)*N;
    Vel += Vboost;
    vel = fromBase(Vel);

    return true;
}

bool graphFuncs::hitPolar_outOnly( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff ) const
{
    sf::Vector2f Pf = Pos - pos;
    Pf = toBase(Pf);
    sf::Vector2f Vel = toBase(vel);

    sf::Vector2f P0 = Pf - Vel*dt;
    float Rf = Length(Pf), R0 = Length(P0);
    if( Rf > yMax  ) return false;// cya when u get here

    float R, drdx;// find y, ypm at P0
    float ang0 = atan2f( P0.y, P0.x );// + x0;
    if( fy && fyPm ) { R = fy(ang0,tElap-dt); drdx = fyPm(ang0,tElap-dt); }
    else if( fy_yPm ) fy_yPm( R, drdx, ang0, tElap-dt );
    else return false;
    float a = R - R0;// distance inside graph (Y>R0) at P0
    // find y, ypm at Pf
    float angF = atan2f( Pf.y, Pf.x );// + x0;
    if( fy && fyPm ) R = fy(angF,tElap);
    else if( fy_yPm ) fy_yPm( R, drdx, angF, tElap );

    float b = Rf - R;// distance outside graph (bRf>Y) at Pf

    // a*b < 0 cases:  a<0 && b>0 = outside before and after. No response required
    if( a < 0.0f && b > 0.0f ) return false;// outside
    // a*b > 0 cases: a<0 && b<0 = crossed to inside and a>0 && b>0 = crossed to outside ( let it go
    if( a > 0.0f && b > 0.0f ) return false;// leaving


    sf::Vector2f Ph;
    float angH = 0.0f;
        // a>0 && b<0 = inside before and after. Place outside
    if( a > 0.0f && b < 0.0f ) Ph = P0 + ( Pf - P0 )*a/( a - b );
    else if( a < 0.0f && b < 0.0f ) Ph = P0 + ( Pf - P0 )*a/( a + b );// other a*b > 0 case: a<0 && b<0 = crossed to inside
    else { std::cout << "\n hitPolar_outOnly says wtf? a= " << a << " b= " << b; return false; }

    // handle hit
    angH = atan2f( Ph.y, Ph.x );// + x0;
    if( fy && fyPm ) { R = fy(angH,tElap); drdx = fyPm(angH,tElap); }
    else if( fy_yPm ) fy_yPm( R, drdx, angH, tElap );
    sf::Vector2f ru( cosf(angH), sinf(angH) );
    Ph = R*ru;
    sf::Vector2f N = getNorm_xyYpm_Local( angH, R, drdx );
    Pos = fromBase( Ph + hitOffset*N ) + pos;
 //   Pos -= stdOffV

    sf::Vector2f au( -ru.y, ru.x );// orig
    sf::Vector2f Vboost = dxdt*R*au;
    if( fdydt ) Vboost += fdydt( angH, tElap )*ru;

    // both sides
    if( a < 0.0f )// bounce off outside
    {
        Vel -= Vboost;
        if( DotProd(Vel,N) < 0.0f ) Vel -= (1.0f + cr)*DotProd(Vel,N)*N;
        Vel += Vboost;
        vel = fromBase(Vel);
    }
    else// just apply boost
        if( DotProd(Vel,N) > 0.0f ) vel = fromBase(Vboost);// push out only. No pulling in

    return true;
}


sf::Vector2f graphFuncs::toBase( sf::Vector2f v ) const// return v in the base Tu, Nu
{
    sf::Vector2f res;
    res.x = v.x*Tu.x + v.y*Tu.y;
    res.y = v.x*Nu.x + v.y*Nu.y;
    return res;
}

sf::Vector2f graphFuncs::fromBase( sf::Vector2f v ) const// back to x, y coords
{
    sf::Vector2f res;
    res.x = v.x*Tu.x + v.y*Nu.x;
    res.y = v.x*Tu.y + v.y*Nu.y;
    return res;
}

// gfEmitter
void gfEmitter::init( std::ifstream& is )
{
    xEmitCurr = 0.0f; tElap = 0.0f; segCurr = 0;
    is >> emitUp >> emitMode >> numSegs;
    is >> tEmit >> emitSpeed >> tLifeEmit;
}

void gfEmitter::init( graphFuncs& gf, std::ifstream& is )// addaed assigns
{
    init(is);
    if( emitMode == 'C' ) { xEmitCurr = gf.xLt; dxEmit = ( gf.xRt - gf.xLt )/numSegs; }
    else if( emitMode == 'O' ) { xEmitCurr = gf.xRt; dxEmit = ( gf.xLt - gf.xRt )/numSegs; }
    else { dxEmit = 0.0f; }
}

bool gfEmitter::update( graphFuncs& gf, sf::Vector2f& Pos, sf::Vector2f& Vel, float dt )// returns true when time to emit
{
    tElap += dt;
    if( tElap >= tEmit )// emit
    {
        tElap -= tEmit;
        xEmitCurr += dxEmit;
        if( emitMode == 'C' )// cyclic
        {
            if( dxEmit > 0.0 && xEmitCurr > gf.xRt ) { xEmitCurr = gf.xLt; }
            if( dxEmit < 0.0 && xEmitCurr < gf.xLt ) { xEmitCurr = gf.xRt; }
        }
    //    if( emitMode == 'O' && xEmitCurr < gf.xLt ) { xEmitCurr = gf.xRt; std::cout << " O"; }// oscillate
        else if( emitMode == 'O' )// oscillate
        {
            if( dxEmit > 0.0 && xEmitCurr > gf.xRt ) { dxEmit *= -1.0f; xEmitCurr = gf.xRt; }
            if( dxEmit < 0.0 && xEmitCurr < gf.xLt ) { dxEmit *= -1.0f; xEmitCurr = gf.xLt; }
        }
        else if( emitMode == 'R' )// random
        {
            static std::default_random_engine rand_gen;
            static std::uniform_real_distribution<float> x_dist(0.0f,1.0f);
            xEmitCurr = gf.xLt + gf.length*x_dist(rand_gen);
        }

        sf::Vector2f Ncurve;
        if( gf.getGlobalMotion( xEmitCurr, Pos, Vel, Ncurve ) )
        {
            if( emitUp ) { Vel += Ncurve*emitSpeed; Pos += Ncurve; }
         //   if( emitUp ) { pSpk->setPosition( spkPos + Ncurve + pSpk->v*dt ); pSpk->v += Ncurve*emitSpeed; }
            else { Vel -= Ncurve*emitSpeed; Pos -= Ncurve; }
            return true;
        }

        return false;// xParam out of range. getGlobalMotion() returned false
    }

    return false;
}

// polyNomial
void polyNomial::init( std::istream& is )
{
    size_t Nterms; is >> Nterms;
    cVec.reserve( Nterms );
    float c;
    for( size_t i = 0; i < Nterms; ++i )
    { is >> c; cVec.push_back(c); }
}

float polyNomial::y( float x ) const
{
    float sum = 0.0f;
    float xPow = 1.0f;
    for( float c : cVec )
    {
        sum += c*xPow;
        xPow *= x;
    }
    return sum;
}

float polyNomial::yPm( float x ) const
{
    float sum = 0.0f;
    float xPow = 1.0f;
    for( size_t i = 1; i < cVec.size(); ++i )
    {
        sum += cVec[i]*xPow*(float)i;
        xPow *= x;
    }
    return sum;
}

void polyNomial::y_yPm( float& Y, float& Ypm, float x ) const
{
    Y = y(x);
    Ypm = yPm(x);
}

// polygonReg
void polygonReg::init( std::istream& is )
{
    is >> Nsides >> R;
    if( Nsides < 3 ) Nsides = 3;
    setNsides( Nsides );
}

void polygonReg::setNsides( int sides )
{
    Nsides = sides > 2 ? sides : 3;
    dAng = myPIx2/Nsides;
    if( dAng < 0.0f ) dAng *= -1.0f;
    angB = ( myPI - dAng )/2.0f;
}

sf::Vector2f polygonReg::ptPos( size_t idx ) const
{
    float angle = (float)idx*dAng;
    return sf::Vector2f( R*cosf(angle), R*sinf(angle) );
}

float polygonReg::r( float x ) const
{
    while( x > dAng ) x -= dAng;
    while( x < 0.0f ) x += dAng;
    return R*sinf( angB )/sinf( myPI - ( x + angB ) );
}

float polygonReg::rPm( float x ) const
{
    while( x > dAng ) x -= dAng;
    while( x < 0.0f ) x += dAng;
    float cosC = cosf( myPI - ( angB + x ) );
    return R*sinf( angB )*cosC/( 1.0f - cosC*cosC );
}

void polygonReg::r_rPm( float& Y, float& Ypm, float x ) const
{
    Y = r(x);
    Ypm = rPm(x);
}

// polygonIrreg
bool polygonIrreg::init( std::istream& is )
{
    size_t Nsides = 0;
    is >> Nsides;
    if( Nsides < 3 ) return false;

    ptVec.reserve( Nsides );
    lengthVec.reserve( Nsides );
    angleVec.reserve( Nsides );
    size_t i = 0;
    sf::Vector2f tmpPt;// pos will be atcentroid
    ctrPt.x = ctrPt.y = 0.0f;
    while( (i < Nsides) && (is >> tmpPt.x >> tmpPt.y) )
    {
        ptVec.push_back( tmpPt );
        ctrPt += tmpPt;
        ++i;
    }

    if( i != Nsides ) { std::cout << "\npgIR init false return i = " << i; return false; }
    // all data gotten
    ctrPt /= (float)Nsides;// find centroid
    for( auto& pt : ptVec )
    {
        pt -= ctrPt;// all point positions relative to centroid
        float R = sqrtf( pt.x*pt.x + pt.y*pt.y );
        lengthVec.push_back( R );
        angleVec.push_back( atan2f( pt.y, pt.x ) );
        std::cout << "\nangle= " << angleVec.back();
        if( R > Rmax ) Rmax = R;
    }

    std::cout << "\nctrPt: " << ctrPt.x << ", " << ctrPt.y << " Rmax = " << Rmax;

 /*   for( i = 0; i + 1 < ptVec.size(); ++i )
    {
        tmpPt = ptVec[i+1] - ptVec[i];
        lengthVec.push_back( sqrtf( tmpPt.x*tmpPt.x + tmpPt.y*tmpPt.y ) );
    }

    // the reach around
    tmpPt = ptVec.front() - ptVec.back();
    lengthVec.push_back( sqrtf( tmpPt.x*tmpPt.x + tmpPt.y*tmpPt.y ) );  */
    return true;
}

float polygonIrreg::r( float x ) const
{
    size_t i = 0;
    float ang = 0.0f;
    if( x < angleVec.front() ) ang = x + myPIx2 - angleVec.back();
    else if( x >= angleVec.back() ) ang = x - angleVec.back();
    else for( i = 1; i < ptVec.size() && x > angleVec[i]; ++i );

    sf::Vector2f r1, r2, r12;
    float r1mag = 1.0f, r12mag;//, r2mag;
    float angB, angC;

    if( i > 0 )
    {
        ang = x - angleVec[i-1];
        r1 = ptVec[i-1];
        r2 = ptVec[i];
        r1mag = lengthVec[i-1];
     //   r2mag = lengthVec[i];
    }
    else// cross over
    {
        r1 = ptVec.back();
        r2 = ptVec.front();
        r1mag = lengthVec.back();
     //   r2mag = lengthVec.front();
    }

    r12 = r2 - r1;
    r12mag = sqrtf( r12.x*r12.x + r12.y*r12.y );
    float sinC = ( r1.x*r2.y - r1.y*r2.x )/(r1mag*r12mag);// r1xr2
    angC = asinf( sinC );
    angB = myPI - ( ang + angC );

    return scale*r1mag*sinC/sinf( angB );// law of sines

 //   std::cout << "\n i= " << i << " x= " << x << " xRel= " << ang;
 //   return scale*Rmax*( 1.0f + 0.1f*i );
}

/*
float polygonIrreg::r( float x ) const
{
    size_t i = 0;
   // x += myPI;
    if( x < angleVec.back() )
        for( i = 0; i < ptVec.size() && x > angleVec[i]; ++i );

    if( i < ptVec.size() )// found start angle
    {
        std::cout << "\n i= " << i << " x= " << x;
        x -= angleVec[i];
        std::cout << " xRel= " << x;
        return scale*Rmax*( 1.0f + 0.1f*i );
    }


    return scale*Rmax;
}
*/

float polygonIrreg::rPm( float x ) const
{
    return 0.0f;
}

void polygonIrreg::r_rPm( float& Y, float& Ypm, float x ) const
{
 //   Y = r(x);
 //   Ypm = rPm(x);

    size_t i = 0;
    float ang = 0.0f;
  //  if( x < angleVec.front() ) ang = x + myPIx2 - angleVec.back();
  //  else if( x >= angleVec.back() ) ang = x - angleVec.back();
  //  else for( i = 1; i < ptVec.size() && x > angleVec[i]; ++i );

    sf::Vector2f r1, r2, r12;
    float r1mag = 1.0f, r12mag;//, r2mag;

    // find i
    sf::Vector2f ru( cosf(x), sinf(x) );
    for( i = 0; i + 1 < ptVec.size(); ++i )
    {
        r1 = ptVec[i];
        r2 = ptVec[i+1];
        if( (ru.x*r1.y-ru.y*r1.x) < 0.0f && (ru.x*r2.y-ru.y*r2.x) > 0.0f )// ratio of cross products ruxr1/ruxr2
        {
            r1mag = lengthVec[i];
         //   r2mag = lengthVec[i+1];
            break;
        }
    }

    if( i + 1 == ptVec.size() )// cross over
    {
        r1 = ptVec.back();
        r2 = ptVec.front();
        r1mag = lengthVec.back();
     //   r2mag = lengthVec.front();
    }

    ang = asinf( (ru.y*r1.x-ru.x*r1.y)/r1mag );
    r12 = r2 - r1;
    r12mag = sqrtf( r12.x*r12.x + r12.y*r12.y );
    float sinC = ( r1.x*r2.y - r1.y*r2.x )/(r1mag*r12mag);// r1xr2
    float angC = asinf( sinC );
    float angB = myPI - ( ang + angC );

    Y = scale*r1mag*sinC/sinf( angB );// law of sines
    Ypm = Y/tanf( angB );
}

/*
void polygonIrreg::r_rPm( float& Y, float& Ypm, float x ) const
{
 //   Y = r(x);
 //   Ypm = rPm(x);

    size_t i = 0;
    float ang = 0.0f;
    if( x < angleVec.front() ) ang = x + myPIx2 - angleVec.back();
    else if( x >= angleVec.back() ) ang = x - angleVec.back();
    else for( i = 1; i < ptVec.size() && x > angleVec[i]; ++i );

    sf::Vector2f r1, r2, r12;
    float r1mag, r2mag, r12mag;
    float angB, angC;

    if( i > 0 )
    {
        ang = x - angleVec[i-1];
        r1 = ptVec[i-1];
        r2 = ptVec[i];
        r1mag = lengthVec[i-1];
        r2mag = lengthVec[i];
    }
    else// cross over
    {
        r1 = ptVec.back();
        r2 = ptVec.front();
        r1mag = lengthVec.back();
        r2mag = lengthVec.front();
    }

    r12 = r2 - r1;
    r12mag = sqrtf( r12.x*r12.x + r12.y*r12.y );
    float sinC = ( r1.x*r2.y - r1.y*r2.x )/(r1mag*r12mag);// r1xr2
    angC = asinf( sinC );
    angB = myPI - ( ang + angC );

    Y = scale*r1mag*sinC/sinf( angB );// law of sines
    Ypm = Y/tanf( angB );
}
*/
