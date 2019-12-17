#ifndef BOMB_H_INCLUDED
#define BOMB_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


class Bomb
{
    public:
    sf::CircleShape ring;
    sf::Color boomColor, BkColor = sf::Color::Black;

    float expPtx = 0.0f, expPty = 0.0f;// explosion point = mseX,mseY on left click. R0 = 0 assigned too.
    float vWave = 10.0f;// and growth rate of R0
    float Reff = 30.0f;// half width of annular force region
    float Rfull = 100.0f;// force applied full until R0 > Rfull. Then inv. prop: *= Rfull/R0
    float R0 = 501.0f;// expPt to wave center
    float Rmax = 500.0f;// max distance from expPt
    float Fapp = 15.0f;// explosion force amplitude


    void update( float dt );
    void draw( sf::RenderTarget& RT ) const;
    bool getForce( float X, float Y, float& Fx, float& Fy )const;
    void init( float Fmax, float v_Wave, float R_full, float R_max, float R_eff );
    void detonate( float X, float Y );
    bool isExploding() const { return R0 < Rmax; }
};


#endif // BOMB_H_INCLUDED
