#include "laser.h"

//float laser::beamLength = 1500.0f;

void laser::init( float x, float y, float DirX, float DirY, float Damage, sf::Color beamColor )
{
    pos.x = x; pos.y = y;
    dir.x = DirX; dir.y = DirY;
    dir = dir.normalize();
    damage = Damage;
    beam[0].position.x = pos.x;
    beam[0].position.y = pos.y;
    beam[1].position.x = pos.x + dir.x*beamLength;
    beam[1].position.y = pos.y + dir.y*beamLength;
    beam[0].color = beam[1].color = beamColor;
    state = false;
}

void laser::update( float x, float y, float DirX, float DirY )
{
    if( !state ) return;
    if( doOscillate ) push = !push;
    pos.x = x; pos.y = y;
    dir.x = DirX; dir.y = DirY;
    dir = dir.normalize();
    beam[0].position.x = pos.x;
    beam[0].position.y = pos.y;
    beam[1].position.x = pos.x + dir.x*beamLength;
    beam[1].position.y = pos.y + dir.y*beamLength;
}

bool laser:: hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass )
{
    if( state )
    {
    //    beam[0].color = beam[1].color = sf::Color::Red;
        vec2f hitPos( hitX, hitY ), hitVel( hitVx, hitVy );
        vec2f sep = hitPos - pos;
        float d = sep.dot( dir );
        if( d > 0.0f && d < beamLength )// is in front of beam
        {
            float h = sep.cross( dir );
            vec2f Nu( dir.y, -dir.x );// RH norm
            if( h < 0.0f )
            {
                Nu *= -1.0f;
                h *= -1.0f;
            }

            if( h < hitR )// hit
            {
          //      hitVel.to_base(dir);
                if( push ) hitVel += damage*dir;// thrust along beam
                else hitVel -= damage*dir;// thrust along beam
                if( beamLock ) hitVel -= hitVel.dot(Nu)*Nu;// remove transverse component
                else hitVel += acc_t*Nu;// thrust out of beam
          //      hitVel.from_base(dir);
                hitVx = hitVel.x; hitVy = hitVel.y;
                beam[0].color = beam[1].color = sf::Color::Green;
                return true;
            }
        }
    }
    return false;
}
