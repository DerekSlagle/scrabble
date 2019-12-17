#ifndef LASER_H_INCLUDED
#define LASER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "utility/vec2f.h"

class laser
{
    public:
    float beamLength = 1500.0f;

    vec2f pos;
    vec2f dir;// unit
    float damage = 0.0f, acc_t = 0.0f;// damage = radial acceleration acc_t > 0 pushes out of beam.
    sf::Vertex beam[2];// to edge of window
    bool state;// on/off
    bool push = true;
    bool doOscillate = true;
    bool beamLock = false;// assign transverve vel component = 0

    laser(){}
    ~laser(){}
    void init( float x, float y, float DirX, float DirY, float Damage, sf::Color beamColor );

    void update( float x, float y, float DirX, float DirY );
    bool hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass );
    void draw( sf::RenderTarget& RT ) const { if( state ) RT.draw( beam, 2, sf::Lines ); }
};

#endif // LASER_H_INCLUDED
