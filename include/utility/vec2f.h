#ifndef VEC2F_H_INCLUDED
#define VEC2F_H_INCLUDED

#include<cmath>

class vec2f
{
    public:
    float x, y;
    vec2f( float X=0.0, float Y=0.0 ): x(X), y(Y) {}
    vec2f& operator +=( const vec2f& v );
    vec2f operator+( const vec2f& v )const;
    vec2f& operator -=( const vec2f& v );
    vec2f operator-( const vec2f& v )const;
    vec2f& operator*=( float c ) { x *= c; y *= c; return *this;}
    vec2f& operator/=( float c ) { x /= c; y /= c; return *this;}
    vec2f operator*( float c )const;
    vec2f operator/( float c )const;

    vec2f operator-() const { return (*this)*-1.0f; }

    float mag()const { return sqrtf( x*x + y*y ); }
    float dot( const vec2f& v )const { return x*v.x + y*v.y; }
    float cross( const vec2f& v )const { return x*v.y - y*v.x; }
    vec2f get_LH_norm()const;

    // coordinate transforms
    vec2f from_base( vec2f baseT )const;// get components in x,y
    vec2f to_base( vec2f baseT )const;// get components in base.x, base.y
    vec2f Rotate( float ang )const;
    vec2f normalize() const { return *this/this->mag(); }
};

vec2f operator*( float c, vec2f v );

class vec3f
{
    public:
    float x=0.0f,y=0.0f,z=0.0f;

    vec3f( float X=0.0f, float Y=0.0f, float Z=0.0f ): x(X), y(Y), z(Z) {}
    void init( float X, float Y, float Z ) { x = X; y = Y; z = Z; }

    vec3f& operator +=( const vec3f& v );
    vec3f operator+( const vec3f& v )const;
    vec3f& operator -=( const vec3f& v );
    vec3f operator-( const vec3f& v )const;
    vec3f& operator*=( float c ) { x *= c; y *= c; z *= c; return *this;}
    vec3f& operator/=( float c ) { x /= c; y /= c; z /= c; return *this;}
    vec3f operator*( float c )const;
    vec3f operator/( float c )const;

    vec3f operator-() const { return (*this)*-1.0f; }

    float mag()const { return sqrtf( x*x + y*y + z*z ); }
    float dot( const vec3f& v )const { return x*v.x + y*v.y + z*v.z; }
    vec3f cross( const vec3f& v )const { return vec3f( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x ); }
    vec3f normalize() const { return *this/this->mag(); }
    vec3f rotate_axis( vec3f axis, float angle )const;

    vec3f from_base( vec3f eu, vec3f ev, vec3f ew )const;// get components in x,y,z
    vec3f to_base( vec3f eu, vec3f ev, vec3f ew )const;// get components in base u,v,w

    void to_spherePolar( float& R, float& angPolar, float& angAzim )const;// values written to args
    void from_spherePolar( float R, float angPolar, float angAzim );// values supplied by args
};

vec3f operator*( float c, vec3f v );

#endif // VEC2F_H_INCLUDED
