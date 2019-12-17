#include "cirLeg.h"

// initial leg
cirLeg::cirLeg( float X0, float Y0, float Xc, float Yc, float Angle )
    : Leg( X0, Y0, 1.0f ), xc(Xc), yc(Yc)
{
    r = sqrtf( (x0-xc)*(x0-xc)+(y0-yc)*(y0-yc) );
    ang0 = atan2f(y0-yc, x0-xc);
//    len = r*Angle;
    len =  Angle < 0.0f ? -r*Angle : r*Angle;
    isNegAngle = Angle < 0.0f;
}

// mid legs
cirLeg::cirLeg( Leg& rPrev, float Xc, float Yc, float Angle )
    : Leg( rPrev.x(rPrev.len), rPrev.y(rPrev.len), 1.0f ), xc(Xc), yc(Yc)
{
    r = sqrtf( (x0-xc)*(x0-xc)+(y0-yc)*(y0-yc) );
    ang0 = atan2f(y0-yc, x0-xc);
    len = r*Angle;
    isNegAngle = Angle < 0.0f;
    if( isNegAngle )
    {
        len *= -1.0f;
    //    ang0 *= -1.0f;
    }
    rPrev.next = this;
    prev = &rPrev;
}

float cirLeg::x( float s )const
{
    if( isNegAngle ) return xc + r*cosf(ang0-s/r);
    return xc + r*cosf(ang0+s/r);
}
float cirLeg::y( float s )const
{
    if( isNegAngle ) return yc + r*sinf(ang0-s/r);
    return yc + r*sinf(ang0+s/r);
}

/*
// mid legs
cirLeg::cirLeg( Leg& rPrev, float Xc, float Yc, float Angle )
    : Leg( rPrev.x(rPrev.len), rPrev.y(rPrev.len), 1.0f ), xc(Xc), yc(Yc)
{
    r = sqrtf( (x0-xc)*(x0-xc)+(y0-yc)*(y0-yc) );
    ang0 = atan2f(y0-yc, x0-xc);
    len = r*Angle;
    rPrev.next = this;
    prev = &rPrev;
}
*/

cirLeg::~cirLeg()
{
    //dtor
}

void cirLeg::T( float s, float& Tx, float& Ty )const// provide unit tangent to curve at s
{
    if( isNegAngle )
    {
        Tx =  sinf(ang0-s/r);
        Ty = -cosf(ang0-s/r);
    }
    else
    {
        Tx = -sinf(ang0+s/r);
        Ty =  cosf(ang0+s/r);
    }
}
