#include "Shot.h"

void Shot::init( float X, float Y, float Vx, float Vy, float Mass, float Rad, float t_Life, sf::Color clr )
{
    x = X; y = Y; vx = Vx; vy = Vy; m = Mass; R = Rad; tLife = t_Life;
    circ.setRadius( R );
    circ.setOrigin( R, R );
    circ.setPosition( x, y );
    circ.setFillColor( clr );
}

bool Shot::hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass )
{
    float Ax = x - vx, Ay = y - vy;// last frame position
    float sepSq = ( hitX - Ax )*( hitX - Ax ) + ( hitY - Ay )*( hitY - Ay );
    float sep = sqrtf( sepSq );
    float sux = ( hitX - Ax )/sep, suy = ( hitY - Ay )/sep;// unit vector hit center to circ center
    if( vx*sux + vy*suy < 0.0f ) return false;// receding

    float vMag = sqrtf( vx*vx + vy*vy );
    float vux = vx/vMag, vuy = vy/vMag;
    float Rs = R + hitR;
    float vDotS = vux*sux + vuy*suy;// unit vectors = cosine
    if( vDotS*vDotS <= 1.0f - Rs*Rs/sepSq ) return false;// will miss

    float a = sep*vDotS - sqrtf( sep*sep*vDotS*vDotS + Rs*Rs - sep*sep );
    if( a > vMag ) return false;// target too far away

    // hit!
    x = Ax + a*vux; y = Ay + a*vuy;
    circ.setPosition(x,y);

    // center of mass
    float mTot = m + hitMass;
    float cmVx = ( m*vx + hitMass*hitVx )/mTot;// velocity
    float cmVy = ( m*vy + hitMass*hitVy )/mTot;
    sux = (x - hitX)/Rs; suy = (y - hitY)/Rs;

    vx -= cmVx; vy -= cmVy;// to cm frame
    vMag = ( 1.0f + Cr )*( vx*sux + vy*suy );// bounce back magnitude
    if( vMag < 0.0f ) { vx -= vMag*sux; vy -= vMag*suy; }// new in cm
    vx += cmVx; vy += cmVy;// back to screen frame

    hitVx -= cmVx; hitVy -= cmVy;//  to cm frame
    vMag = ( 1.0f + Cr )*( hitVx*sux + hitVy*suy );// bounce back
    hitVx -= vMag*sux; hitVy -= vMag*suy;// new in cm
    hitVx += cmVx; hitVy += cmVy;// back from cm frame

    return true;
}

// ShotBomb

void ShotBomb::draw( sf::RenderTarget& RT ) const
{
    if( tLife > 0.0f ) RT.draw( circ );
    else if( myBomb.isExploding() ) myBomb.draw(RT);
}

void ShotBomb::update( float dt )
{
    if( !inUse() ) return;
    if( tLife > 0.0f )
    {
        tLife -= dt;
        x += vx; y += vy; circ.setPosition(x,y);
        if( tLife <= 0.0f )// go boom!
            myBomb.detonate(x,y);
    }
    else myBomb.update(dt);
}

void guidedShot::update( float dt )
{
    if( !inUse() ) return;
    if( !pTgtX ) { return Shot::update(dt); }
    if( tLife > 0.0f )
    {
        tLife -= dt;
        if( tLife <= 0.0f ) { pTgtX = pTgtY = pTgtVx = pTgtVy = nullptr; return; }

        float vMag = sqrtf( vx*vx + vy*vy );
        float vux = vx/vMag, vuy = vy/vMag;
   //     v += a_t*vu;
        float sepx = *pTgtX - x, sepy = *pTgtY - y;
        float sepMag = sqrtf( sepx*sepx + sepy*sepy );
    //    if( sepMag > R )
        {
            float sux = sepx/sepMag, suy = sepy/sepMag;
            float vnx = -vy, vny = vx ;
             // apply accel
        //    v += a_p*vn*vu.cross(sepu);// proport to sine of angle towards target

            vx += a_p*vnx*( vux*suy - vuy*sux );// proport to sine of angle towards target
            vy += a_p*vny*( vux*suy - vuy*sux );// proport to sine of angle towards target

            float vMag1 = sqrtf( vx*vx + vy*vy );
            vx *= ( vMag + a_t )/vMag1;
            vy *= ( vMag + a_t )/vMag1;
       //     std::cout << '*';
        }

        x += vx; y += vy; circ.setPosition(x,y);
    }
}

void clusterShot::update( float dt )
{
    if( !inUse() ) return;
    Shot::update(dt);

    if( tLife <= 0.0f && pGetShot )// time to frag
    {
        float dAngle = 6.283185f/numFrags;
        Shot* pShot = nullptr;
        for( unsigned int i=0; i< numFrags; ++i )
        {

            pShot = pGetShot();
            if( pShot ) pShot->init( x, y, vFrags*cosf(i*dAngle), vFrags*sinf(i*dAngle), m/2.0f, R/2.0f, tLifeFrags, sf::Color::Green );
        }
    }
}
