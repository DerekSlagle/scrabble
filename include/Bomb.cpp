#include "Bomb.h"

void Bomb::init( float Fmax, float v_Wave, float R_full, float R_max, float R_eff )
{
    Fapp = Fmax; vWave = v_Wave;
    Rfull = R_full;
    Rmax = R_max;
    Reff = R_eff;
    R0 = 1.0f + Rmax;// not exploding

    boomColor.r = 255; boomColor.a = 200; boomColor.g = boomColor.b = 0;
}

void Bomb::update( float dt )
{
    if( R0 > Rmax ) return;
    R0 += vWave*dt;
    ring.setRadius( R0 + Reff );
    ring.setOrigin( R0 + Reff, R0 + Reff );
    sf::Color ringColor;
    ringColor.r = floor( boomColor.r*( 1.0f - R0/Rmax ) );
    ringColor.a = floor( boomColor.a*( 1.0f - R0/Rmax ) );
    ringColor.b = floor( 200*R0/Rmax );
    ring.setOutlineThickness( -2.0f*Reff );
    ring.setOutlineColor( ringColor );
    ring.setFillColor( BkColor );
    ring.setPosition( expPtx, expPty );
}

void Bomb::detonate( float X, float Y )
{
    R0 = vWave;//0.0f;
    expPtx = X;
    expPty = Y;
    ring.setRadius( R0 + Reff );
    ring.setOrigin( R0 + Reff, R0 + Reff );
    ring.setFillColor( boomColor );
    ring.setPosition( X, Y );
}

void Bomb::draw( sf::RenderTarget& RT ) const
{
    if( R0 < Rmax ) RT.draw( ring );
}

bool Bomb::getForce( float X, float Y, float& Fx, float& Fy )const
{
    if( R0 > Rmax ) return false;

    // is tgt in annular region?
    float sepx = X - expPtx, sepy = Y - expPty;
    float Rmh = sqrtf( sepx*sepx + sepy*sepy );
    if( Rmh < 1.0f ) Rmh = 1.0f;// safe for div by 0 below
    if( Rmh > R0-Reff && Rmh < R0+Reff )
    {
       if( R0 > Rfull ){ Fx = (Fapp/Rmh)*(Rfull/R0)*sepx; Fy = (Fapp/Rmh)*(Rfull/R0)*sepy; }
       else { Fx = (Fapp/Rmh)*sepx; Fy = (Fapp/Rmh)*sepy; }
       return true;
    }

    return false;
}
