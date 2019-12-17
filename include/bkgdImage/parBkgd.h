#ifndef PARBKGD_H
#define PARBKGD_H

#include "scrollImage.h"
#include "BkgdArr.h"
#include<vector>
#include<string>
#include<fstream>
using std::vector;
using std::string;


class parBkgd
{
    public:
    static sf::Rect<float>& rGR;
    static const float& g_scale;
    sf::Image img;
    vector<scrollImage> scrImgVec;// single strip layers
    vector<BkgdArr*> p_layerVec;// element layers
    vector<char> layerTypeVec;// holds order of layer types for drawing
    bool valid;

    // functions
    void move(void);
    void draw(void);
    void setColor( const sf::Color& newColor );// used to fade out the entire background (all layers)
    void setSpeed( float Vx );

    parBkgd();
    parBkgd( string config_fname );
    ~parBkgd();
};

#endif // PARBKGD_H
