#ifndef BKGDARR_H
#define BKGDARR_H

#include <SFML/Graphics.hpp>
#include<iostream>
#include<fstream>
#include<vector>
using std::cout;
using std::endl;

struct BKelement
{
    sf::Sprite spr;
    float sepX;

    BKelement( const sf::Texture& rTxt, int left, int right, int top, int bottom, float SepX )
    {
        spr.setTexture( rTxt );
        sf::Rect<int> src( left, top, right, bottom );
        spr.setTextureRect( src );
        sepX = SepX;
    }

    BKelement(){}

    void INIT( const sf::Texture& rTxt, int left, int right, int top, int bottom, float SepX, float x, float y, float Scale )
    {
        spr.setTexture( rTxt );
        sf::Rect<int> src( left, top, right - left, bottom - top );
        spr.setTextureRect( src );
        spr.setOrigin( 0.0f, spr.getLocalBounds().height );
        spr.setScale( Scale, Scale );
        spr.setPosition( x, y );
        sepX = SepX;
    }
};

class BkgdArr
{
    public:

    std::vector<BKelement> pEle;
    static sf::Rect<float> rGR;
 //   static sf::RenderWindow* pWndw;
    int nEle = 0;
    float vx = 0.0f;
    float vFactor = 1.0f;

    int idxLo = 0;
    int idxHi = 0;

    // functions
    void update( float dt );
    void draw( sf::RenderTarget& RT ) const;
    void setColor( const sf::Color& newColor );// used to fade out a bkgd layer

    BkgdArr(){}
    void init( const sf::Texture& rTxt, float Scale, std::ifstream& fin );
    BkgdArr( const sf::Texture& rTxt, float Scale, std::ifstream& fin ) { init( rTxt, Scale, fin ); }
    ~BkgdArr(){}
};

#endif // BKGDARR_H
