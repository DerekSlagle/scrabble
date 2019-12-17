#include "perspectivePoint.h"

float persPt::X0 = 0.0f;
float persPt::Yh = 200.0f, persPt::Yg = 500.0f;
float persPt::Z0 = 1000.0f;
float persPt::Xv = 0.0f, persPt::Yv = 0.0f, persPt::Zv = 0.0f;

void persPt::init( float Xs, float Ys, float Zs )
{
    xs = Xs; ys = Ys; zs = Zs;
    float zFactor = Z0/zs;
    x = xs*zFactor;
    y =  Y_0( zs ) - ys*zFactor;
}

void persPt::setXvanish()// assigns vtx.position.x
{
    x = ( xs - persPt::Xv )*persPt::Z0/zs;
}

float persPt::zFact( float y )
{
    return ( y - persPt::Yh )/( persPt::Yg - persPt::Yh );
}

// persPack
vec3f persPack::viewPt( persPt::X0, persPt::Yh, 0.0f );

/*
void persPack::setPosition( float Xs, float Ys, float Zs )
{
 //   Xs -= persPt::Xv;
    vec3f newPos(Xs,Ys,Zs);
    vec3f dPos = newPos - pos;
    dPos.x -= persPt::Xv;
    for( auto& ps : pt ) { ps.init( ps.xs + dPos.x, ps.ys + dPos.y, ps.zs + dPos.z ); ps.xs += persPt::Xv; }
    for( unsigned int i = 0; i < vtx.size(); ++i ) vtx[i].position = pt[ vtxIdx[i] ].screenPos();
    pos = newPos;
    pos -= persPt::Xv;
    setDistSq();
    pos += persPt::Xv;
}


bool persPack::fromFile( std::istream& is )
{
    is >> pos.x >> pos.y >> pos.z;
    pos.z += persPt::Z0;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    sf::Color inClr( rd, gn, bu );

    unsigned int numPts;
    is >> numPts; pt.resize( numPts );
    vec3f ptPos;
    ctrOfst.x = ctrOfst.y = ctrOfst.z = 0.0f;
    for( unsigned int i = 0; i < numPts; ++i )
    {
        is >> ptPos.x >> ptPos.y >> ptPos.z;
        pt[i].init( ptPos + pos );
        ctrOfst += ptPos;
    }
    ctrOfst /= (float)numPts;

    unsigned int numVtx, idx=0;
    is >> numVtx; vtx.resize( numVtx ); vtxIdx.resize( numVtx );
    for( unsigned int i = 0; i < numVtx; ++i )
    {
        is >> idx;
        vtxIdx[i] = idx;
        vtx[i].position = pt[idx].screenPos();
        vtx[i].color = inClr;
    }

    return is.good();
}   */

void persPack::draw( sf::RenderTarget& RT ) const
{
    RT.draw( &vtx[0], vtx.size(), sf::Lines );
}

void persPack::setXvanish()
{
    for( auto& ps : pt ) ps.setXvanish();
    for( unsigned int i = 0; i < vtx.size(); ++i ) vtx[i].position.x = pt[ vtxIdx[i] ].x + persPt::X0;
    pos.x -= persPt::Xv;
    setDistSq();
    pos.x += persPt::Xv;
}

// persMatrix
void persMatrix::setPosition( float Xs, float Ys, float Zs )
{
    Xs -= persPt::Xv;
    pos.x = Xs; pos.y = Ys; pos.z = Zs;

    for( unsigned int i = 0; i < pt.size(); ++i ) { pt[i].init( pos.x + ptPos[i].x, pos.y + ptPos[i].y, pos.z + ptPos[i].z ); }
    for( unsigned int i = 0; i < vtx.size(); ++i ) vtx[i].position = pt[ vtxIdx[i] ].screenPos();

    setDistSq();
    pos += persPt::Xv;
}

bool persMatrix::fromFile( std::istream& is )
{
    is >> pos.x >> pos.y >> pos.z;
    pos.z += persPt::Z0;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    sf::Color inClr( rd, gn, bu );

    unsigned int numPts;
    is >> numPts;
    pt.resize( numPts ); ptPos.resize( numPts );
 //   vec3f ptPosTmp;
    ctrOfst.x = ctrOfst.y = ctrOfst.z = 0.0f;
    for( unsigned int i = 0; i < numPts; ++i )
    {
        is >> ptPos[i].x >> ptPos[i].y >> ptPos[i].z;
        pt[i].init( ptPos[i] + pos );
        ctrOfst += ptPos[i];
    }
    ctrOfst /= (float)numPts;

    unsigned int numVtx, idx=0;
    is >> numVtx; vtx.resize( numVtx ); vtxIdx.resize( numVtx );
    for( unsigned int i = 0; i < numVtx; ++i )
    {
        is >> idx;
        vtxIdx[i] = idx;
        vtx[i].position = pt[idx].screenPos();
        vtx[i].color = inClr;
    }

    return is.good();
}

void persMatrix::rotate_axis( vec3f axis, float angle )// modifies ptPos
{
    for( auto& v : ptPos ) v = v.rotate_axis( axis, angle );
    setPosition( pos.x, pos.y, pos.z );
}

// persQuad - derived from persPack
void persQuad::setTxtRect( sf::IntRect srcRect, char Ta, char Tb )
{
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

    vtx[ iSt[0] ].texCoords.x = srcRect.left;// up lt
    vtx[ iSt[0] ].texCoords.y = srcRect.top;
    vtx[ iSt[1] ].texCoords.x = srcRect.left + srcRect.width;// up rt
    vtx[ iSt[1] ].texCoords.y = srcRect.top;
    vtx[ iSt[2] ].texCoords.x = srcRect.left + srcRect.width;// dn rt
    vtx[ iSt[2] ].texCoords.y = srcRect.top + srcRect.height;
    vtx[ iSt[3] ].texCoords.x = srcRect.left;// dn lt
    vtx[ iSt[3] ].texCoords.y = srcRect.top + srcRect.height;
}

void persQuad::setPosition( float Xs, float Ys, float Zs )
{
    Xs -= persPt::Xv;
    pos.x = Xs; pos.y = Ys; pos.z = Zs;
    const vec3f xu(1.0f,0.0f,0.0f), yu(0.0f,1.0f,0.0f), zu(0.0f,0.0f,1.0f);// unit y
    vec3f u = sfcNorm.cross( yu );// horizontal - width applies
    float uMag = u.mag();
    if( uMag > 0.1f ) u /= uMag;// unit Length
    else u = xu;
    vec3f v = u.cross( sfcNorm );// vertical - height applies
    // front
    pt[0].init( pos );// bot lt         0,0
    pt[1].init( pos + v*H );// up lt      v*H
    pt[2].init( pos + v*H + u*W );// up rt  v*H + u*W
    pt[3].init( pos + u*W );// bot rt     u*W

    for( unsigned int i = 0; i < 4; ++i )
    {
        vtx[i].position.x = pt[i].x + persPt::X0;
        vtx[i].position.y = pt[i].y;
    }

    setDistSq();
    pos.x += persPt::Xv;
}

void persQuad::init( float Xs, float Ys, float Zs, float width, float height, vec3f sfcNormal, sf::Color clr )
{
    W = width; H = height;
    for( unsigned int i = 0; i < 4; ++i ) vtx[i].color = clr;
    sfcNorm = sfcNormal.normalize();
    setPosition( Xs, Ys, Zs );
    ctrOfst.x = pt[2].xs - pt[0].xs;
    ctrOfst.y = pt[2].ys - pt[0].ys;
    ctrOfst.z = pt[2].zs - pt[0].zs;
    ctrOfst *= 0.5f;
}

void persQuad::setXvanish()
{
    for( unsigned int i = 0; i < 4; ++i )
    {
        pt[i].setXvanish();
        vtx[i].position.x = pt[i].x + persPt::X0;
    }

    pos.x -= persPt::Xv;
    setDistSq();
    pos.x += persPt::Xv;
}

void persQuad::draw( sf::RenderTarget& RT ) const
{
    if( pTxt ) RT.draw( &vtx[0], 4, sf::Quads, pTxt );
    else RT.draw( &vtx[0], 4, sf::Quads );
}

bool persQuad::fromFile( std::istream& is )
{
    is >> pos.x >> pos.y >> pos.z;
    pos.z += persPt::Z0;
    is >> W >> H;
    is >> sfcNorm.x >> sfcNorm.y >> sfcNorm.z;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    init( pos.x, pos.y, pos.z, W, H, sfcNorm, sf::Color(rd,gn,bu) );
    return is.good();
}

bool persQuad::fromFile( std::istream& is, spriteSheet& rSS )
{
    is >> pos.x >> pos.y >> pos.z;
    pos.z += persPt::Z0;
    is >> W >> H;
    is >> sfcNorm.x >> sfcNorm.y >> sfcNorm.z;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    init( pos.x, pos.y, pos.z, W, H, sfcNorm, sf::Color(rd,gn,bu) );
    pTxt = &rSS.txt;
    unsigned int Set = 0,frIdx = 0;
    is >> Set >> frIdx;
    char xf1, xf2; is >> xf1 >> xf2;
    setTxtRect( rSS.getFrRect(frIdx,Set), xf1, xf2 );
    return is.good();
}

void persQuad::toFile( std::ostream& os ) const
{
    os << pos.x << ' ' << pos.y << ' ' << pos.z - persPt::Z0;
    os << '\n' << W << ' ' << H;
    os << '\n' << (unsigned)vtx[0].color.r << ' ' << (unsigned)vtx[0].color.g << ' ' << (unsigned)vtx[0].color.b << '\n';
}

// persBox
void persBox::setTxtRect( sf::IntRect srcRect, vec3f faceNormal, char Ta, char Tb )
{
    if( !( drawFaces && pTxt ) ) return;
    unsigned int idx = 0;// base index to vtx[]
    if( Tfr.dot( faceNormal ) == 1.0f ) idx = 0;// front
    else if( Tbk.dot( faceNormal ) == 1.0f ) idx = 4;// back
    else if( Ttp.dot( faceNormal ) == 1.0f ) idx = 8;// top
    else if( Tbt.dot( faceNormal ) == 1.0f ) idx = 12;// bottom
    else if( Trt.dot( faceNormal ) == 1.0f ) idx = 16;// right
    else if( Tlt.dot( faceNormal ) == 1.0f ) idx = 20;// left
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
    Xs -= persPt::Xv;
    pos.x = Xs; pos.y = Ys; pos.z = Zs;

    // front
    pt[0].init( pos + Ttp*dimY );// up lt - Ft
    pt[1].init( pos + Trt*dimX + Ttp*dimY );// up rt - Ft
    pt[2].init( pos + Trt*dimX );// bot rt - Ft
    pt[3].init( pos );// bot lt - Ft
    // back

    pt[4].init( pos + Ttp*dimY + Tbk*dimZ );// up lt - Bk
    pt[5].init( pos + Trt*dimX + Ttp*dimY + Tbk*dimZ );// up rt - Bk
    pt[6].init( pos + Trt*dimX + Tbk*dimZ );// bot rt - Bk
    pt[7].init( pos + Tbk*dimZ );// bot lt - Bk

    setVtxPositions();
    setDistSq();
    pos.x += persPt::Xv;
}

void persBox::setVtxPositions()
{
    if( drawFaces )
    {
        // front + back
        for( unsigned int i = 0; i < 8; ++i ) vtx[i].position = pt[i].screenPos();
        // top
        vtx[8].position = pt[0].screenPos();// up lt - Ft
        vtx[9].position = pt[4].screenPos();// up lt - Bk
        vtx[10].position = pt[5].screenPos();// up rt - Bk
        vtx[11].position = pt[1].screenPos();// up rt - Ft
        // bottom
        vtx[12].position = pt[3].screenPos();// bot lt - Ft
        vtx[13].position = pt[7].screenPos();// bot lt - Bk
        vtx[14].position = pt[6].screenPos();// bot rt - Bk
        vtx[15].position = pt[2].screenPos();// bot rt - Ft
        // right
        vtx[16].position = pt[1].screenPos();// up rt - Ft
        vtx[17].position = pt[5].screenPos();// up rt - Bk
        vtx[18].position = pt[6].screenPos();// bot rt - Bk
        vtx[19].position = pt[2].screenPos();// bot rt - Ft
         // left
        vtx[20].position = pt[0].screenPos();// up lt - Ft
        vtx[21].position = pt[4].screenPos();// up lt - Bk
        vtx[22].position = pt[7].screenPos();// bot lt - Bk
        vtx[23].position = pt[3].screenPos();// bot lt - Ft
    }
    else// 12 lines
    {
         // front
        // 0 =  up lt - Ft
        // 1 = up rt - Ft
        // 2 = bot rt - Ft
        // 3 =  bot lt - Ft
        // back
        // 4 = up lt - Bk
        // 5 = up rt - Bk
        // 6 = bot rt - Bk
        // 7 = bot lt - Bk

        // front face - 4 lines
        vtx[0].position = pt[3].screenPos();// bot lt - Ft
        vtx[1].position = pt[0].screenPos();// up lt - Ft
        vtx[2].position = pt[0].screenPos();// up lt
        vtx[3].position = pt[1].screenPos();// up rt - Ft
        vtx[4].position = pt[1].screenPos();// up rt
        vtx[5].position = pt[2].screenPos();// bot rt - Ft
        vtx[6].position = pt[2].screenPos();// bot rt
        vtx[7].position = pt[3].screenPos();// bot lt
        // back face - 4 lines
        vtx[8].position = pt[7].screenPos();// bot lt - Bk
        vtx[9].position = pt[4].screenPos();// up lt - Bk
        vtx[10].position = pt[4].screenPos();// up lt
        vtx[11].position = pt[5].screenPos();// up rt - Bk
        vtx[12].position = pt[5].screenPos();// up rt
        vtx[13].position = pt[6].screenPos();// bot rt - Bk
        vtx[14].position = pt[6].screenPos();// bot rt
        vtx[15].position = pt[7].screenPos();// bot lt
        // bottom edges
        vtx[16].position = pt[3].screenPos();// bot lt - Ft
        vtx[17].position = pt[7].screenPos();// bot lt - Bk
        vtx[18].position = pt[2].screenPos();// bot rt - Ft
        vtx[19].position = pt[6].screenPos();// bot rt - Bk
        vtx[20].position = pt[0].screenPos();// up lt - Ft
        vtx[21].position = pt[4].screenPos();// up lt - Bk
        vtx[22].position = pt[1].screenPos();// up rt - Ft
        vtx[23].position = pt[5].screenPos();// up rt - Bk
    }
}

/*
void persBox::setPosition( float Xs, float Ys, float Zs )
{
    Xs -= persPt::Xv;
    pos.x = Xs; pos.y = Ys; pos.z = Zs;

    // front
    pt[0].init( pos );// bot lt - Ft
    pt[1].init( pos + Ttp*dimY );// up lt - Ft
    pt[2].init( pos + Trt*dimX + Ttp*dimY );// up rt - Ft
    pt[3].init( pos + Trt*dimX );// bot rt - Ft
    // back
    pt[4].init( pos + Tbk*dimZ );// bot lt - Bk
    pt[5].init( pos + Ttp*dimY + Tbk*dimZ );// up lt - Bk
    pt[6].init( pos + Trt*dimX + Ttp*dimY + Tbk*dimZ );// up rt - Bk
    pt[7].init( pos + Trt*dimX + Tbk*dimZ );// bot rt - Bk

    setVtxPositions();
    setDistSq();
    pos.x += persPt::Xv;
}

void persBox::setVtxPositions()
{
    if( drawFaces )
    {
        // front + back
        for( unsigned int i = 0; i < 8; ++i ) vtx[i].position = pt[i].screenPos();
        // top
        vtx[8].position = pt[1].screenPos();// up lt - Ft
        vtx[9].position = pt[5].screenPos();// up lt - Bk
        vtx[10].position = pt[6].screenPos();// up rt - Bk
        vtx[11].position = pt[2].screenPos();// up rt - Ft
        // bottom
        vtx[12].position = pt[0].screenPos();// bot lt - Ft
        vtx[13].position = pt[4].screenPos();// bot lt - Bk
        vtx[14].position = pt[7].screenPos();// bot rt - Bk
        vtx[15].position = pt[3].screenPos();// bot rt - Ft
        // right
        vtx[16].position = pt[2].screenPos();// up rt - Ft
        vtx[17].position = pt[6].screenPos();// up rt - Bk
        vtx[18].position = pt[7].screenPos();// bot rt - Bk
        vtx[19].position = pt[3].screenPos();// bot rt - Ft
         // left
        vtx[20].position = pt[1].screenPos();// up lt - Ft
        vtx[21].position = pt[5].screenPos();// up lt - Bk
        vtx[22].position = pt[4].screenPos();// bot lt - Bk
        vtx[23].position = pt[0].screenPos();// bot lt - Ft
    }
    else// 12 lines
    {
         // front
        // 0 =  bot lt - Ft
        // 1 =  up lt - Ft
        // 2 = up rt - Ft
        // 3 = bot rt - Ft
        // back
        // 4 = bot lt - Bk
        // 5 = up lt - Bk
        // 6 = up rt - Bk
        // 7 = bot rt - Bk

        // front face - 4 lines
        vtx[0].position = pt[0].screenPos();// bot lt - Ft
        vtx[1].position = pt[1].screenPos();// up lt - Ft
        vtx[2].position = pt[1].screenPos();// up lt
        vtx[3].position = pt[2].screenPos();// up rt - Ft
        vtx[4].position = pt[2].screenPos();// up rt
        vtx[5].position = pt[3].screenPos();// bot rt - Ft
        vtx[6].position = pt[3].screenPos();// bot rt
        vtx[7].position = pt[0].screenPos();// bot lt
        // back face - 4 lines
        vtx[8].position = pt[4].screenPos();// bot lt - Bk
        vtx[9].position = pt[5].screenPos();// up lt - Bk
        vtx[10].position = pt[5].screenPos();// up lt
        vtx[11].position = pt[6].screenPos();// up rt - Bk
        vtx[12].position = pt[6].screenPos();// up rt
        vtx[13].position = pt[7].screenPos();// bot rt - Bk
        vtx[14].position = pt[7].screenPos();// bot rt
        vtx[15].position = pt[4].screenPos();// bot lt
        // bottom edges
        vtx[16].position = pt[0].screenPos();// bot lt - Ft
        vtx[17].position = pt[4].screenPos();// bot lt - Bk
        vtx[18].position = pt[3].screenPos();// bot rt - Ft
        vtx[19].position = pt[7].screenPos();// bot rt - Bk
        vtx[20].position = pt[1].screenPos();// up lt - Ft
        vtx[21].position = pt[5].screenPos();// up lt - Bk
        vtx[22].position = pt[2].screenPos();// up rt - Ft
        vtx[23].position = pt[6].screenPos();// up rt - Bk
    }
}
*/

void persBox::init( float Xs, float Ys, float Zs, float DimX, float DimY, float DimZ, sf::Color clr, bool DrawFaces )
{
    drawFaces = DrawFaces;
    dimX = DimX; dimY = DimY; dimZ = DimZ;
    for( unsigned int i = 0; i < 24; ++i ) vtx[i].color = clr;

    Tfr = vec3f(0.0f,0.0f,-1.0f);// front
    Tbk = vec3f(0.0f,0.0f,1.0f);// back
    Ttp = vec3f(0.0f,1.0f,0.0f);// top
    Tbt = vec3f(0.0f,-1.0f,0.0f);// bottom
    Trt = vec3f(1.0f,0.0f,0.0f);// right
    Tlt = vec3f(-1.0f,0.0f,0.0f);// left

    setPosition( Xs, Ys, Zs );

    ctrOfst.x = pt[6].xs - pt[0].xs;
    ctrOfst.y = pt[6].ys - pt[0].ys;
    ctrOfst.z = pt[6].zs - pt[0].zs;
    ctrOfst *= 0.5f;
}

void persBox::setXvanish()
{
    for( unsigned int i = 0; i < 8; ++i ) pt[i].setXvanish();
    setVtxPositions();
    pos.x -= persPt::Xv;
    setDistSq();
    pos.x += persPt::Xv;
}

void persBox::draw( sf::RenderTarget& RT ) const
{
    if( drawFaces )
    {
        if( pTxt )
        {
            vec3f relPos( persPack::viewPt );
            relPos.z -= + pos.z;

            persPt ctr( pos + ctrOfst );
       //     relPos.x -= ctr.x + persPt::X0 - persPt::Xv;// best
            relPos.x -= ctr.x + persPt::X0 - persPt::Xv*persPt::Z0/pos.z;
            relPos.y -= ctr.y; relPos.y *= -1.0f;

            // by face
            if( relPos.dot( Tfr ) > 0.0f ) RT.draw( &vtx[0], 4, sf::Quads, pTxt );// front
            else if( relPos.dot( Tbk ) > 0.0f ) RT.draw( &vtx[4], 4, sf::Quads, pTxt );// back

            if( relPos.dot( Ttp ) > 0.0f ) RT.draw( &vtx[8], 4, sf::Quads, pTxt );// top
            else if( relPos.dot( Tbt ) > 0.0f ) RT.draw( &vtx[12], 4, sf::Quads, pTxt );// bottom

            if( relPos.dot( Trt ) > 0.0f ) RT.draw( &vtx[16], 4, sf::Quads, pTxt );// right
            else if( relPos.dot( Tlt ) > 0.0f ) RT.draw( &vtx[20], 4, sf::Quads, pTxt );// left
        }
        else RT.draw( &vtx[0], 24, sf::Quads );
    }
    else RT.draw( &vtx[0], 24, sf::Lines );
}

bool persBox::fromFile( std::istream& is, bool DrawFaces )
{
    is >> pos.x >> pos.y >> pos.z;
    pos.z += persPt::Z0;
    is >> dimX >> dimY >> dimZ;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    init( pos.x, pos.y, pos.z, dimX, dimY, dimZ, sf::Color(rd,gn,bu), DrawFaces );
    return is.good();
}

bool persBox::fromFile( std::istream& is, spriteSheet& rSS )
{
    is >> pos.x >> pos.y >> pos.z;
    pos.z += persPt::Z0;
    is >> dimX >> dimY >> dimZ;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;// color
    init( pos.x, pos.y, pos.z, dimX, dimY, dimZ, sf::Color(rd,gn,bu), true );
    pTxt = &rSS.txt;
    unsigned int Set = 0,frIdx = 0;
    is >> Set;
    is >> frIdx; setTxtRect( rSS.getFrRect(frIdx,Set), vec3f(0.0f,0.0f,-1.0f) );// front face = brick 25
    is >> frIdx; setTxtRect( rSS.getFrRect(frIdx,Set), vec3f(0.0f,0.0f,1.0f) );// back face = water 11
    is >> frIdx; setTxtRect( rSS.getFrRect(frIdx,Set), vec3f(0.0f,1.0f,0.0f) );// top face = grass 10
    is >> frIdx; setTxtRect( rSS.getFrRect(frIdx,Set), vec3f(0.0f,-1.0f,0.0f) );// bottom face = rocks+dirt 8
    is >> frIdx; setTxtRect( rSS.getFrRect(frIdx,Set), vec3f(-1.0f,0.0f,0.0f) );// Left face = stone 24
    is >> frIdx; setTxtRect( rSS.getFrRect(frIdx,Set), vec3f(1.0f,0.0f,0.0f) );// Right face = stripes 55
    return is.good();
}

void persBox::toFile( std::ostream& os ) const
{
    os << pos.x << ' ' << pos.y << ' ' << pos.z - persPt::Z0;
    os << '\n' << dimX << ' ' << dimY << ' ' << dimZ;
    os << '\n' << (unsigned)vtx[0].color.r << ' ' << (unsigned)vtx[0].color.g << ' ' << (unsigned)vtx[0].color.b << '\n';
}
/*
persBox& persBox::operator=( const persBox& bx )// arrays don't copy themselves!
{
    drawFaces = bx.drawFaces;
    pTxt = bx.pTxt;
    dimX = bx.dimX; dimY = bx.dimY; dimZ = bx.dimZ;
    pos = bx.pos;
    unsigned int i = 0;
    for( i = 0; i < 6; ++i ) sfcNorm[i] = bx.sfcNorm[i];
    for( i = 0; i < 24; ++i ) vtx[i] = bx.vtx[i];
    for( i = 0; i < 8; ++i ) pt[i] = bx.pt[i];
    return *this;
}   */

void persBoxMover::update( float dt )
{
    pos += v*dt;
    setPosition( pos.x, pos.y, pos.z );
}

bool persBoxMover::keepInBounds( float Left, float Right, float Top, float Bottom, float Front, float Back )
{
     bool Hit = false;

    if( pos.x < Left ) { if( v.x < 0.0f ) { v.x *= -1.0f; Hit = true; } }
    else if( pos.x + dimX > Right ) { if( v.x > 0.0f ){ v.x *= -1.0f; Hit = true; } }

    if( pos.y + dimY < Top ) { if( v.y < 0.0f ) { v.y *= -1.0f; Hit = true; } }
    else if( pos.y > Bottom ) { if( v.y > 0.0f ) { v.y *= -1.0f; Hit = true; } }

    if( pos.z > Back ) { if( v.z > 0.0f ) { v.z *= -1.0f; Hit = true; } }
    else if( pos.z < Front ) { if( v.z < 0.0f ) { v.z *= -1.0f; Hit = true; } }

    return Hit;
}
