#ifndef SHOT_H_INCLUDED
#define SHOT_H_INCLUDED

#include<iostream>
#include<functional>
#include "Bomb.h"

class Shot
{
    public:
    float x=0.0f, y=0.0f;
    float vx=0.0f, vy=0.0f;
    float m=1.0f, R=1.0f, Cr = 0.8f;
    float tLife = 0.0f;// lifetime
    sf::CircleShape circ;

    virtual void draw( sf::RenderTarget& RT ) const { if( tLife > 0.0f ) RT.draw( circ ); }
    virtual void update( float dt ){ if( tLife <= 0.0f ) return; tLife -= dt; if( tLife > 0.0f ){ x += vx; y += vy; circ.setPosition(x,y); } }
    void setPosition( float X, float Y ) { x = X; y = Y; circ.setPosition(X,Y); }
    void init( float X, float Y, float Vx, float Vy, float Mass, float Rad, float t_Life, sf::Color clr );
  //  { x = X; y = Y; vx = Vx; vy = Vy; m = Mass; R = Rad; tLife = t_Life; }
    virtual void reset( float X, float Y, float Vx, float Vy, float t_Life ) { x = X; y = Y; vx = Vx; vy = Vy; tLife = t_Life; }
    virtual bool hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass );
    virtual bool inUse() const { return tLife > 0.0f; }
};

class ShotBomb : public Shot
{
    public:
    Bomb myBomb;

    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void update( float dt );
    virtual bool inUse() const { return myBomb.isExploding() || tLife > 0.0f; }
};

class guidedShot : public Shot
{
    public:
    float a_t = 0.0f, a_p = 1.0f;
    float *pTgtX = nullptr, *pTgtY = nullptr;// for tracking in update()
    float *pTgtVx = nullptr, *pTgtVy = nullptr;// for tracking in update()

    // base versions ok for: draw, setPosition, init (may overload), inUse
    virtual void update( float dt );
 //   virtual void reset( float X, float Y, float Vx, float Vy, float t_Life ) { Shot::reset(X,Y,Vx,Vy,t_Life); pTgtX = pTgtY = pTgtVx = pTgtVy = nullptr; }
    virtual bool hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass )
    {
        if( Shot::hitFreeLine(hitX,hitY,hitVx,hitVy,hitR,hitMass ) && pTgtVx == &hitVx )
            { pTgtX = pTgtY = pTgtVx = pTgtVy = nullptr; return true; }
        return false;
    }
};


class clusterShot : public Shot
{
    public:
    float vFrags = 30.0f;
    float tLifeFrags = 30.0f;
    unsigned int numFrags = 7;
    std::function<Shot*(void)> pGetShot = nullptr;
 //   bool onFire( target* p_tgt ) { if( shot::onFire() ) { pTgt = p_tgt; return true;} return false; }
    virtual void update( float dt );
};

#endif // SHOT_H_INCLUDED
