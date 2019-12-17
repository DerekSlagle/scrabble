#ifndef CIRLEG_H
#define CIRLEG_H

#include "Leg.h"

class cirLeg : public Leg
{
    public:
        float xc, yc;
        float r, ang0;
        bool isNegAngle;
        cirLeg( float X0, float Y0, float Xc, float Yc, float Angle );// 1st leg
        cirLeg( Leg& rPrev, float Xc, float Yc, float Angle );// mid legs

        virtual float x( float s )const;
        virtual float y( float s )const;
        virtual ~cirLeg();

        virtual void T( float s, float& Tx, float& Ty )const;// provide unit tangent to curve at s

    protected:

    private:
};

#endif // CIRLEG_H
