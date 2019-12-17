#ifndef GRAPHFUNCS_H_INCLUDED
#define GRAPHFUNCS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include<vector>
#include<functional>

#include "button_types/button.h"
//#include <string>
//#include "mvHit_types/vec2d.h"

struct graphFuncs
{
    static float hitOffset;

    std::vector<sf::Vertex> vtxVec;
    sf::Vertex xAxis[6];// includes nifty arrow in +x direction
    sf::Vertex yAxis[6];// includes nifty arrow in +y direction
    sf::Text xAxisLabel, yAxisLabel;
    sf::Color gfColor = sf::Color::Cyan;
    float xLt, xRt, x0;// x0 = expansion center
    float yMin, yMax;// to pre test for hit
    float tElap = 0.0f, tPeriod = -1.0f;// not periodic if < 0
    float dxdt = 0.0f;// shifting domain. Update: xLt, x0, xRt all += dxtd*dt

    float length;
    float sclX = 1.0f, sclY = 1.0f;
    float dxVtx = 3.0f;// distance between sf::Vertex
    bool held = false;// for dragging
    bool showAxes = true;
    char coordType = 'C';// cartesian or polar = 'P'
    int hitSide = 0;// 0 = both sides, +1 = outside, -1 = inside (only)

//    vec2d pos;// base position corresponds to x = xLt, y = 0
    sf::Vector2f pos;// base position corresponds to x = xLt, y = 0
    sf::Vector2f Tu, Nu;// base coords
    sf::Vector2f toBase( sf::Vector2f v ) const;// return v in the base Tu, Nu
    sf::Vector2f fromBase( sf::Vector2f v ) const;// back to x, y coords
    sf::Vector2f getNorm_xyYpm_Local( float x, float y, float ypm ) const;// in Tu, Tv
    sf::Vector2f getNorm_xyYpm( float x, float y, float ypm ) const;// in x, y
    sf::Vector2f getNorm_xt( float x, float t ) const;
    sf::Vector2f getPosition_xt( float x, float t ) const;
    sf::Vector2f getPosition_xy( float x, float y ) const;
    void set_dxdt( float dxDt );
    void setLength( float L );// for coordType == 'C' only

    // cartesian or polar coords
    std::function<float(float,float)> fy = nullptr;// time by rfc so can maintain range
    std::function<float(float,float)> fyPm = nullptr;
    std::function<void(float&,float&,float,float)> fy_yPm = nullptr;// to rep both above

    // drawing from discrete points
    std::function<sf::Vector2f(size_t)> fPt_n = nullptr;
    std::function<size_t(void)> fnumPts = nullptr;

    // polar coords
 //   std::function<float(float,float)> fr = nullptr;// time by rfc so can maintain range
 //   std::function<float(float,float)> fdrdx = nullptr;

    std::function<float(float,float)> fdydt = nullptr;// explicit t dependence for y(x) or r(x)

    void draw( sf::RenderTarget& RT ) const;// draws axes + scaled and positioned function
    bool hit( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff = 0.0f ) const;
    bool hitPolar( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff = 0.0f ) const;
    bool hitPolar_outOnly( sf::Vector2f& Pos, sf::Vector2f& vel, float cr, float dt, float standOff = 0.0f ) const;
    sf::Vector2f getPosition() const { return pos; }
    bool getGlobalMotion( float xParam, sf::Vector2f& ptPos, sf::Vector2f& ptVel, sf::Vector2f& Ncurve ) const;
    void setPosition( sf::Vector2f Pos );
    void setGraph() { setPosition( pos ); } // replot at same position
    void setRotation( float angle );
    void update( float dt );
    bool drag( bool wLeft = true );
    graphFuncs() {}

    void init( std::istream& is );
    void init( std::istream& is, std::function<void(float&,float&,float,float)> Fy_yPm, std::function<float(float,float)> Fdydt );
    graphFuncs( std::istream& is, std::function<void(float&,float&,float,float)> Fy_yPm, std::function<float(float,float)> Fdydt ) { init(is,Fy_yPm,Fdydt); }

    void init( std::istream& is, std::function<float(float,float)> Fy , std::function<float(float,float)> FyPm, std::function<float(float,float)> Fdydt );
    graphFuncs( std::istream& is, std::function<float(float,float)> Fy , std::function<float(float,float)> FyPm, std::function<float(float,float)> Fdydt ) { init(is,Fy,FyPm,Fdydt); }
    ~graphFuncs() {}
};

struct gfEmitter
{
    char emitMode = 'C';// 'C' = cyclic, 'O' = oscillat, 'R' = random
    float xEmitCurr = 0.0f, dxEmit = 10.0f;
    int numSegs = 10, segCurr = 0;// dxEmit = (xRt-xLt)/numSegs
    float tElap = 0.0f, tEmit = 1.0f, tLifeEmit = 100.0f;
    bool emitUp = true;
    float emitSpeed = 4.0f;
    bool update( graphFuncs& gf, sf::Vector2f& Pos, sf::Vector2f& Vel, float dt );// returns true when time to emit
    void init( std::ifstream& is );
    void init( graphFuncs& gf, std::ifstream& is );// addaed assigns
};

struct polyNomial
{
    std::vector<float> cVec;
    void init( std::istream& is );
    float y( float x ) const;
    float yPm( float x ) const;
    void y_yPm( float& Y, float& Ypm, float x ) const;
};

struct polygonReg// polar function
{
    unsigned int Nsides = 3;// minimum #
    float R = 1.0f;// spoke length
    float dAng = 1.0f, angB;// 2*pi/Nsides, ( pi - dAng )/2.0f
    void init( std::istream& is );
    void setNsides( int sides );
    sf::Vector2f ptPos( size_t idx ) const;// position of point
    size_t numPts(void) const { return Nsides; }
    float r( float x ) const;// x = polar angle
    float rPm( float x ) const;// dr/dx
    void r_rPm( float& Y, float& Ypm, float x ) const;
};

struct polygonIrreg// polar function
{
    std::vector<sf::Vector2f> ptVec;// Nsides = ptVec.size()
    sf::Vector2f ctrPt;// Nsides = ptVec.size()
    float scale = 1.0f;
    std::vector<float> lengthVec, angleVec;// Nsides = ptVec.size()
    float Rmax = 0.0f;
    bool init( std::istream& is );
    sf::Vector2f ptPos( size_t idx ) const { return scale*( ptVec[idx] ); }// position of point
    size_t numPts(void) const { return ptVec.size(); }
    float r( float x ) const;// x = polar angle
    float rPm( float x ) const;// dr/dx
    void r_rPm( float& Y, float& Ypm, float x ) const;
};

#endif // GRAPHFUNCS_H_INCLUDED
