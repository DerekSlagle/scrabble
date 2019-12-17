#ifndef PERSPECTIVEPOINT_H_INCLUDED
#define PERSPECTIVEPOINT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include "vec2f.h"

struct persPt
{
    float xs, ys, zs, y0;//, zs;
    float x, y;// screen coords
    static float X0, Yh, Yg, Z0;
    static float zFact( float y );

    persPt(){}
    void init( float Xs, float Ys, float Y0 );
    persPt( float Xs, float Ys, float Y0 ) { init( Xs, Ys, Y0 ); }
    void setXvanish( float Xv );// assigns vtx.position.x
    float dY0dz() const { return -(Yg-Yh)*Z0/(zs*zs); }
    float z( float Y0 ) const { return Z0*( Yg - Yh )/( Y0 - Yh ); }
    float Y0( float z )const { return ( Yg - Yh )*zs/Z0 + Yh; }
};

struct persBox
{
    bool drawFaces = false;// or 12 edges
    sf::Texture* pTxt = nullptr;// can use if drawFaces
    vec3f sfcNorm[6];// outward normal to each box face
    sf::Vertex vtx[24];
    persPt pt[24];// enough for 12 sf::Lines or 6 sf::Quads
    float dimX=0.0f, dimY=0.0f, dimZ=0.0f;
    vec3f pos;// torep above

    void init( float Xs, float Ys, float Y0, float DimX, float DimY, float DimZ, sf::Color clr, bool DrawFaces = false );
    void setPosition( float Xs, float Ys, float Zs );
    void setXvanish( float Xv );
    void draw( sf::RenderTarget& RT ) const;
    void draw( sf::RenderTarget& RT, vec3f viewPt ) const;
    void setTxtRect( sf::IntRect srcRect, vec3f faceNormal, char Ta = 'n', char Tb = 'n' );// transform args: Ta = 'R': rotata, 'F': flip, Tb = '1','2','3','X','Y'
    bool fromFile( std::istream& is, bool DrawFaces = false );
    void toFile( std::ostream& os ) const;

    float dY0dz() const { return -( persPt::Yg - persPt::Yh )*persPt::Z0/(pos.z*pos.z); }
    float Y_0( float z )const { return ( persPt::Yg - persPt::Yh )*persPt::Z0/z + persPt::Yh; }

    persBox& operator=( const persBox& bx );
    persBox( const persBox& bx ) { *this = bx; }
    persBox(){}
};

struct persBoxMover : persBox
{
    vec3f v;
    void update( float Xv, float dt );
    bool keepInBounds( float Left, float Right, float Top, float Bottom, float Front, float Back );
};


#endif // PERSPECTIVEPOINT_H_INCLUDED
