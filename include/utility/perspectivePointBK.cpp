#include "perspectivePoint.h"

float persPt::X0 = 0.0f, persPt::Yh = 200.0f, persPt::Yg = 500.0f;
float persPt::Z0 = 1000.0f;

void persPt::init( float Xs, float Ys, float Y0 )
{
    xs = Xs; ys = Ys; y0 = Y0;
    float zFactor = ( y0 - Yh )/( Yg - Yh );
    x = xs*zFactor;
    y = y0 - ys*zFactor;
    zs = Z0/zFactor;

 //   y0 = ( Yg - Yh )*zs/Z0 + Yh;
}

void persPt::setXvanish( float Xv )// assigns vtx.position.x
{
    x = ( xs - Xv )*( y0 - Yh )/( Yg - Yh );
}

float persPt::zFact( float y )
{
    return ( y - persPt::Yh )/( persPt::Yg - persPt::Yh );
}

void persBox::setTxtRect( sf::IntRect srcRect, vec3f faceNormal, char Ta, char Tb )
{
    if( !( drawFaces && pTxt ) ) return;
    unsigned int idx = 0;// base index to vtx[]
    if( sfcNorm[0].dot( faceNormal ) == 1.0f ) idx = 0;// front
    else if( sfcNorm[1].dot( faceNormal ) == 1.0f ) idx = 4;// back
    else if( sfcNorm[2].dot( faceNormal ) == 1.0f ) idx = 8;// top
    else if( sfcNorm[3].dot( faceNormal ) == 1.0f ) idx = 12;// bottom
    else if( sfcNorm[4].dot( faceNormal ) == 1.0f ) idx = 16;// right
    else if( sfcNorm[5].dot( faceNormal ) == 1.0f ) idx = 20;// left
    else return;// no match

    unsigned int iSt[4] = {0,1,2,3};
    if( Ta == 'R' )// rotate
    {
        while( Tb-- > '0' ) for( size_t i = 0; i < 4; ++i ) ++iSt[i];
        for( size_t i = 0; i < 4; ++i ) iSt[i] %= 4;
    }
    else if( Ta == 'F' )
    {
        if( Tb == 'X' ) { iSt[0] = 1; iSt[1] = 0; iSt[2] = 3; iSt[3] = 2; }
        else if( Tb == 'Y' ) { iSt[0] = 2; iSt[1] = 3; iSt[2] = 0; iSt[3] = 1; }
    }

    vtx[ idx+iSt[0] ].texCoords.x = srcRect.left;// up lt
    vtx[ idx+iSt[0] ].texCoords.y = srcRect.top;
    vtx[ idx+iSt[1] ].texCoords.x = srcRect.left + srcRect.width;// up rt
    vtx[ idx+iSt[1] ].texCoords.y = srcRect.top;
    vtx[ idx+iSt[2] ].texCoords.x = srcRect.left + srcRect.width;// dn rt
    vtx[ idx+iSt[2] ].texCoords.y = srcRect.top + srcRect.height;
    vtx[ idx+iSt[3] ].texCoords.x = srcRect.left;// dn lt
    vtx[ idx+iSt[3] ].texCoords.y = srcRect.top + srcRect.height;
}

void persBox::setPosition( float Xs, float Ys, float Zs )
{
    init( Xs, Ys, Zs, dimX, dimY, dimZ, vtx[0].color, drawFaces );
}

void persBox::init( float Xs, float Ys, float Y0, float DimX, float DimY, float DimZ, sf::Color clr, bool DrawFaces )
{
    drawFaces = DrawFaces;
    dimX = DimX; dimY = DimY; dimZ = DimZ;
 //   posX = Xs; posY = Ys; posZ = Y0;
    pos.x = Xs; pos.y = Ys; pos.z = Y0;

    float zFactor = ( Y0 - persPt::Yh )/( persPt::Yg - persPt::Yh );
//    pos.z = persPt::Z0/zFactor;// NEW
    DimZ *= zFactor;
    float dy0 = DimZ*( Y0 - persPt::Yh )/sqrtf( Xs*Xs + (Ys-persPt::Yh)*(Ys-persPt::Yh) + (Y0-persPt::Yh)*(Y0-persPt::Yh) );

    sfcNorm[0] = vec3f(0.0f,0.0f,1.0f);// front
    sfcNorm[1] = vec3f(0.0f,0.0f,-1.0f);// back
    sfcNorm[2] = vec3f(0.0f,-1.0f,0.0f);// top
    sfcNorm[3] = vec3f(0.0f,1.0f,0.0f);// bottom
    sfcNorm[4] = vec3f(1.0f,0.0f,0.0f);// right
    sfcNorm[5] = vec3f(-1.0f,0.0f,0.0f);// left

    if( drawFaces )
    {
        // front
        pt[0].init( Xs, Ys, Y0 );// bot lt - Ft
        pt[1].init( Xs, Ys + dimY, Y0 );// up lt - Ft
        pt[2].init( Xs + dimX, Ys + dimY, Y0 );// up rt - Ft
        pt[3].init( Xs + dimX, Ys, Y0 );// bot rt - Ft
        // back
        pt[4].init( Xs, Ys, Y0 - dy0 );// bot lt - Bk
        pt[5].init( Xs, Ys + dimY, Y0 - dy0 );// up lt - Bk
        pt[6].init( Xs + dimX, Ys + dimY, Y0 - dy0 );// up rt - Bk
        pt[7].init( Xs + dimX, Ys, Y0 - dy0 );// bot rt - Bk
        // top
        pt[8].init( Xs, Ys + dimY, Y0 );// up lt - Ft
        pt[9].init( Xs, Ys + dimY, Y0 - dy0 );// up lt - Bk
        pt[10].init( Xs + dimX, Ys + dimY, Y0 - dy0 );// up rt - Bk
        pt[11].init( Xs + dimX, Ys + dimY, Y0 );// up rt - Ft
        // bottom
        pt[12].init( Xs, Ys, Y0 );// bot lt - Ft
        pt[13].init( Xs, Ys, Y0 - dy0 );// bot lt - Bk
        pt[14].init( Xs + dimX, Ys, Y0 - dy0 );// bot rt - Bk
        pt[15].init( Xs + dimX, Ys, Y0 );// bot rt - Ft
        // right
        pt[16].init( Xs + dimX, Ys + dimY, Y0 );// up rt - Ft
        pt[17].init( Xs + dimX, Ys + dimY, Y0 - dy0 );// up rt - Bk
        pt[18].init( Xs + dimX, Ys, Y0 - dy0 );// bot rt - Bk
        pt[19].init( Xs + dimX, Ys, Y0 );// bot rt - Ft
         // left
        pt[20].init( Xs, Ys + dimY, Y0 );// up lt - Ft
        pt[21].init( Xs, Ys + dimY, Y0 - dy0 );// up lt - Bk
        pt[22].init( Xs, Ys, Y0 - dy0 );// bot lt - Bk
        pt[23].init( Xs, Ys, Y0 );// bot lt - Ft
    }
    else// 12 lines
    {
        // front face - 4 lines
        pt[0].init( Xs, Ys, Y0 );// bot lt - Ft
        pt[1].init( Xs, Ys + dimY, Y0 );// up lt - Ft
        pt[2].init( Xs, Ys + dimY, Y0 );// up lt
        pt[3].init( Xs + dimX, Ys + dimY, Y0 );// up rt - Ft
        pt[4].init( Xs + dimX, Ys + dimY, Y0 );// up rt
        pt[5].init( Xs + dimX, Ys, Y0 );// bot rt - Ft
        pt[6].init( Xs + dimX, Ys, Y0 );// bot rt
        pt[7].init( Xs, Ys, Y0 );// bot lt
        // back face - 4 lines
        pt[8].init( Xs, Ys, Y0 - dy0 );// bot lt - Bk
        pt[9].init( Xs, Ys + dimY, Y0 - dy0 );// up lt - Bk
        pt[10].init( Xs, Ys + dimY, Y0 - dy0 );// up lt
        pt[11].init( Xs + dimX, Ys + dimY, Y0 - dy0 );// up rt - Bk
        pt[12].init( Xs + dimX, Ys + dimY, Y0 - dy0 );// up rt
        pt[13].init( Xs + dimX, Ys, Y0 - dy0 );// bot rt - Bk
        pt[14].init( Xs + dimX, Ys, Y0 - dy0 );// bot rt
        pt[15].init( Xs, Ys, Y0 - dy0 );// bot lt
        // bottom edges
        pt[16].init( Xs, Ys, Y0 );// bot lt - Ft
        pt[17].init( Xs, Ys, Y0 - dy0 );// bot lt - Bk
        pt[18].init( Xs + dimX, Ys, Y0 );// bot rt - Ft
        pt[19].init( Xs + dimX, Ys, Y0 - dy0 );// bot rt - Bk
        pt[20].init( Xs, Ys + dimY, Y0 );// up lt - Ft
        pt[21].init( Xs, Ys + dimY, Y0 - dy0 );// up lt - Bk
        pt[22].init( Xs + dimX, Ys + dimY, Y0 );// up rt - Ft
        pt[23].init( Xs + dimX, Ys + dimY, Y0 - dy0 );// up rt - Bk
    }

    for( unsigned int i = 0; i < 24; ++i )
    {
        vtx[i].position.x = pt[i].x + persPt::X0;
        vtx[i].position.y = pt[i].y;
        vtx[i].color = clr;
    }
}

void persBox::setXvanish( float Xv )
{
    for( unsigned int i = 0; i < 24; ++i )
    {
        pt[i].setXvanish( Xv );
        vtx[i].position.x = pt[i].x + persPt::X0;
    }
}

void persBox::draw( sf::RenderTarget& RT ) const
{
    if( drawFaces )
    {
        if( pTxt ) RT.draw( vtx, 24, sf::Quads, pTxt );
        else RT.draw( vtx, 24, sf::Quads );
    }
    else RT.draw( vtx, 24, sf::Lines );
}

void persBox::draw( sf::RenderTarget& RT, vec3f viewPt ) const
{
    if( drawFaces )
    {
        if( pTxt )
        {
            vec3f relPos( viewPt );
            relPos.x -= pt[0].x + persPt::X0;
            relPos.y -= pt[0].y;
            relPos.z += pos.z;
            // by face
            if( relPos.dot( sfcNorm[0] ) > 0.0f ) RT.draw( vtx, 4, sf::Quads, pTxt );// front
            if( relPos.dot( sfcNorm[1] ) > 0.0f ) RT.draw( vtx + 4, 4, sf::Quads, pTxt );// back
            if( relPos.dot( sfcNorm[2] ) > 0.0f ) RT.draw( vtx + 8, 4, sf::Quads, pTxt );// top
            if( relPos.dot( sfcNorm[3] ) > 0.0f ) RT.draw( vtx + 12, 4, sf::Quads, pTxt );// bottom
            if( relPos.dot( sfcNorm[4] ) > 0.0f ) RT.draw( vtx + 16, 4, sf::Quads, pTxt );// right
            if( relPos.dot( sfcNorm[5] ) > 0.0f ) RT.draw( vtx + 20, 4, sf::Quads, pTxt );// left
        }
        else RT.draw( vtx, 24, sf::Quads );
    }
    else RT.draw( vtx, 24, sf::Lines );
}

bool persBox::fromFile( std::istream& is, bool DrawFaces )
{
    float Y0;
    is >> pos.x >> pos.y >> Y0;
    pos.z = Y0;
    is >> dimX >> dimY >> dimZ;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    init( pos.x, pos.y, pos.z, dimX, dimY, dimZ, sf::Color(rd,gn,bu), DrawFaces );
    return is.good();
}

void persBox::toFile( std::ostream& os ) const
{
    float Y0 = pos.z;
    os << pos.x << ' ' << pos.y << ' ' << Y0;
    os << '\n' << dimX << ' ' << dimY << ' ' << dimZ;
    os << '\n' << (unsigned)vtx[0].color.r << ' ' << (unsigned)vtx[0].color.g << ' ' << (unsigned)vtx[0].color.b << '\n';
}

persBox& persBox::operator=( const persBox& bx )// arrays don't copy themselves!
{
    drawFaces = bx.drawFaces;
    pTxt = bx.pTxt;
    dimX = bx.dimX; dimY = bx.dimY; dimZ = bx.dimZ;
    pos = bx.pos;
    unsigned int i = 0;
    for( i = 0; i < 6; ++i ) sfcNorm[i] = bx.sfcNorm[i];
    for( i = 0; i < 24; ++i ) vtx[i] = bx.vtx[i];
    for( i = 0; i < 24; ++i ) pt[i] = bx.pt[i];
    return *this;
}

void persBoxMover::update( float Xv, float dt )
{
    pos.z += v.z*persPt::zFact( pos.z )*dt;
 //   pos.z += v.z*dt;
    pos.x += v.x*dt;
    pos.y += v.y*dt;
    float Y0 = pos.z;//Y_0( pos.z );
    float PosX = pos.x;
    setPosition( pos.x - Xv, pos.y, Y0 );
    pos.x = PosX;
}

bool persBoxMover::keepInBounds( float Left, float Right, float Top, float Bottom, float Front, float Back )
{
     bool Hit = false;

    if( pos.x < Left ) { if( v.x < 0.0f ) { v.x *= -1.0f; Hit = true; } }
    else if( pos.x + dimX > Right ) { if( v.x > 0.0f ){ v.x *= -1.0f; Hit = true; } }

    if( pos.y + dimY < Top ) { if( v.y < 0.0f ) { v.y *= -1.0f; Hit = true; } }
    else if( pos.y > Bottom ) { if( v.y > 0.0f ) { v.y *= -1.0f; Hit = true; } }

    if( pos.z < Back ) { if( v.z < 0.0f ) { v.z *= -1.0f; Hit = true; } }
    else if( pos.z > Front ) { if( v.z > 0.0f ) { v.z *= -1.0f; Hit = true; } }

    return Hit;
}
