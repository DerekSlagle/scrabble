#ifndef PERSPECTIVEPOINT_H_INCLUDED
#define PERSPECTIVEPOINT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include "vec2f.h"
#include "../spriteSheet.h"

struct persPt
{

    static float X0, Yh, Yg, Z0;
    static float Xv, Yv, Zv;// Xv = view shift along axis-
    static float angPolar;
    static float zFact( float y );
    static float z( float Y0 ) { return Z0*( Yg - Yh )/( Y0 - Yh ); }// + persPt::Zv; }
    static float Y_0( float z ) { return ( Yg - Yh )*Z0/( z - persPt::Zv ) + Yh; }
 //   static float dY0dz( float z ) { return -(Yg-Yh)*Z0/(z*z); }

    float xs, ys, zs;//, y0;//, zs;// scene coords
    float x, y;// screen coords

    persPt(){}
    void init( float Xs, float Ys, float Zs );
    void init( vec3f pos ) { init( pos.x, pos.y, pos.z ); }
    persPt( float Xs, float Ys, float Zs ) { init( Xs, Ys, Zs ); }
    persPt( vec3f pos ) { init( pos.x, pos.y, pos.z ); }
    void setXvanish();// assigns vtx.position.x
    // utility
    void updateScreenPos();
    sf::Vector2f screenPos() const { return sf::Vector2f( x, y ); }
    vec3f scenePos() const { return vec3f(xs,ys,zs); }
};

struct persPack
{
    static vec3f viewPt;
    std::vector<sf::Vertex> vtx;
    std::vector<unsigned int> vtxIdx;// index to persPt for position
    std::vector<persPt> pt;// enough for 12 sf::Lines or 6 sf::Quads
    vec3f pos, ctrOfst;// offset to center from pos
    float distSq = 0.0f;// squared distance from viewPt
    void setDistSq() { vec3f PP(persPt::Xv,persPt::Yv,persPt::Zv), sep( pos + ctrOfst - persPack::viewPt - PP ); distSq = sep.dot( sep ); }
    static bool compare( persPack* pa, persPack* pb ) { return pa->distSq > pb->distSq; }

    virtual void setPosition( float Xs, float Ys, float Zs ) = 0;
    virtual void setVtxPositions() = 0;
    virtual void update();// over ride in persQuadAni
    void updatePts() { for( persPt& ps : pt ) ps.updateScreenPos(); }
    void setPosition( vec3f pos ) { setPosition( pos.x, pos.y, pos.z ); }
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual bool fromFile( std::istream& is ) = 0;
    virtual void setXvanish();
};

struct persMatrix : persPack
{
    std::vector<vec3f> ptPos;// displacement to each persPt from pos

    virtual void setPosition( float Xs, float Ys, float Zs );
    virtual void setVtxPositions();
    virtual bool fromFile( std::istream& is );
    void rotate_axis( vec3f axis, float angle );// modifies ptPos
};

struct persQuad : persPack
{
    sf::Texture* pTxt = nullptr;// can use if drawFaces
    vec3f sfcNorm;// outward normal to each box face
  //  std::vector<sf::Vertex> vtx;
  //  std::vector<persPt> pt;// enough for 12 sf::Lines or 6 sf::Quads
    float W=0.0f, H=0.0f;
 //   vec3f pos;

    void init( float Xs, float Ys, float Zs, float width, float height, vec3f sfcNormal, sf::Color clr );
    virtual void setPosition( float Xs, float Ys, float Zs );
    virtual void setVtxPositions();
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual bool fromFile( std::istream& is );
    virtual bool fromFile( std::istream& is, spriteSheet& rSS );
    virtual void setXvanish();

    void setTxtRect( sf::IntRect srcRect, char Ta = 'n', char Tb = 'n' );// transform args: Ta = 'R': rotata, 'F': flip, Tb = '1','2','3','X','Y'
    void toFile( std::ostream& os ) const;
    persQuad(){ vtx.resize(4);  pt.resize(4); }
};

struct persQuadAni : persQuad
{
    spriteSheet* pSS = nullptr;
    size_t setNum = 0, frIdx = 0;
    size_t delayCnt = 0, frDelay = 1;// counter, limit
    virtual bool fromFile( std::istream& is, spriteSheet& rSS );
    virtual void update();// load next frame
};

struct persBox : persPack
{
    bool drawFaces = false;// or 12 edges
    sf::Texture* pTxt = nullptr;// can use if drawFaces
    vec3f Tfr, Tbk, Tlt, Trt, Ttp, Tbt;
    float dimX=0.0f, dimY=0.0f, dimZ=0.0f;

    void init( float Xs, float Ys, float Zs, float DimX, float DimY, float DimZ, sf::Color clr, bool DrawFaces = false );

    virtual void setPosition( float Xs, float Ys, float Zs );
    virtual void setVtxPositions();
    virtual void draw( sf::RenderTarget& RT ) const;
    bool fromFile( std::istream& is, bool DrawFaces );
    bool fromFile( std::istream& is, spriteSheet& rSS );
    virtual bool fromFile( std::istream& is ) {  return fromFile( is, false ); }
    virtual void setXvanish();
 //   void setPosition( vec3f pos ) { setPosition( pos.x, pos.y, pos.z ); }
    void setPosition( vec3f pos ) { setPosition( pos.x, pos.y, pos.z ); }
    void setTxtRect( sf::IntRect srcRect, vec3f faceNormal, char Ta = 'n', char Tb = 'n' );// transform args: Ta = 'R': rotata, 'F': flip, Tb = '1','2','3','X','Y'
    void toFile( std::ostream& os ) const;

 //   persBox& operator=( const persBox& bx );
    persBox(){ vtx.resize(24);   pt.resize(8); }
 //   persBox( const persBox& bx ): persBox() { *this = bx; }

};

struct persBoxMover : persBox
{
    vec3f v;
    void updateMove( float dt );
    bool keepInBounds( float Left, float Right, float Top, float Bottom, float Front, float Back );
};


#endif // PERSPECTIVEPOINT_H_INCLUDED
