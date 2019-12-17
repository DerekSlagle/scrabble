#ifndef SCROLLIMAGE_H
#define SCROLLIMAGE_H

#include <SFML/Graphics.hpp>
#include<fstream>

class scrollImage
{
    public:

    sf::Sprite sprL;
    sf::Sprite sprR;
    float x_left;
    float elev;
    float vx;
    float vFactor;
    bool drawRight;

    float x0, y0;
    int W, H;

    sf::Rect<int> src;
    static sf::Rect<float> rGR;
 //   static const float& g_scale;
    int imgSzX;
    int imgSzY;


    // functions
    int move(void);
    void draw( sf::RenderTarget& RT ) const;
    bool INIT( const sf::Texture& rTxt, int left, int right, int top, int bottom, float v_Factor, float Elev );
    void setColor( const sf::Color& newColor );// used to fade out the scrolling background

    // new - for vertical scrolling
 //   void INIT_vert( const sf::Texture& rTxt, sf::Rect<int> src_rect, float X0, float Y0, int width, int height, float v, float scroll_0 = 0.0f );
 //   void move_vert(void);

    scrollImage();
    scrollImage( const sf::Texture& rTxt, std::ifstream& fin );
    ~scrollImage();
};

#endif // SCROLLIMAGE_H
