#ifndef DIALDISPLAY_H_INCLUDED
#define DIALDISPLAY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>


class dialDisplay
{
    public:
    sf::Texture *pDialTxt = nullptr, *pNeedleTxt = nullptr;
    sf::Sprite dialSpr, needleSpr;
    sf::Vector2f pos, needleOfst, labelOfst;
    sf::Text label;

    float minVal, maxVal, currVal;
    float minAngle = -0.5f, maxAngle = 0.5f;

    bool init( sf::Vector2f Pos, const char* fName );
    void setPosition( sf::Vector2f Pos );
    void setPosition( float x, float y ) { setPosition( sf::Vector2f(x,y) ); }
    void update( float dt );
    void draw( sf::RenderTarget& RT ) const { RT.draw( dialSpr ); RT.draw( needleSpr ); RT.draw( label ); }
    void setValue( float val );
};

#endif // DIALDISPLAY_H_INCLUDED
