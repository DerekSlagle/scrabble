#include "pathUtils.h"

void makePath( std::ifstream& inFile, Leg*& pLeg )
{
    float x0, y0, x1, y1, angle;
    char legType = 'n';// 'L'=linLeg, 'C'=cirLeg
    // 1st Leg takes full params
    inFile >> legType;
    if( legType == 'L' )
    {
        inFile >> x0 >> y0 >> x1 >> y1;
        pLeg = new linLeg( x0, y0, x1, y1 );// 1st leg
    }
    else if( legType == 'C' )
    {
        inFile >> x0 >> y0 >> x1 >> y1 >> angle;
        pLeg = new cirLeg( x0, y0, x1, y1, angle );// 1st leg
    }
    else if( legType == 'F' )
    {
        inFile >> x0 >> y0 >> x1 >> y1 >> angle;// angle = K = deflection
        pLeg = new flexLeg( x0, y0, x1, y1, angle );// 1st leg
    }

    int legCnt = 1;// must be > 1 for closed and >2 for closed with entryLeg
    Leg* pLeg0 = pLeg;// path will close on pLeg0, pLeg0->next or nullptr
    while( inFile >> legType )
    {
        if( legType == 'X' ) break;// last leg

        if( legType == 'L' )
        {
            inFile >> x1 >> y1;
            pLeg->next = new linLeg( *pLeg, x1, y1 );// mid legs
        }
        else if( legType == 'C' )
        {
            inFile >> x1 >> y1 >> angle;
            pLeg->next = new cirLeg( *pLeg, x1, y1, angle );// mid legs
        }
        else if( legType == 'F' )
        {
            inFile >> x1 >> y1 >> angle;
            pLeg->next = new flexLeg( *pLeg, x1, y1, angle );// mid legs
        }
        pLeg = pLeg->next;
        ++legCnt;
    }

    // last Leg
    if(  inFile && legType == 'X' )
    {
        char homeIdx = '0';
        inFile >> homeIdx >> legType;
        if( legType == 'L' )
        {
        //    if( entryLeg ){ pLeg->next = new linLeg( *pLeg, pLeg0->next->x(0.0f), pLeg0->next->y(0.0f) ); } else
            if( legCnt > 0 && homeIdx == '0' ) pLeg->next = new linLeg( *pLeg, *pLeg0 );// closed, no entry leg
            else if( legCnt > 1 && homeIdx == '1' ) pLeg->next = new linLeg( *pLeg, *(pLeg0->next) );// with entryLeg
            else pLeg->next = nullptr;// open at last leg
        }
        else if( legType == 'C' )
        {
            inFile >> x1 >> y1 >> angle;
            if( (legCnt > 0 && homeIdx == '0') || (legCnt > 1 && homeIdx == '1') )
            {
                pLeg->next = new cirLeg( *pLeg, x1, y1, angle );// mid legs
                pLeg = pLeg->next;// now pointing to last Leg
                if( homeIdx == '0' ) { pLeg->next = pLeg0; pLeg0->prev = pLeg; }// closed no entryLeg
                if( homeIdx == '1' ) { pLeg->next = pLeg0->next; pLeg0->next->prev = pLeg; }// closed with entryLeg
            }
            else pLeg->next = nullptr;// open at last leg
        }

        if( pLeg->next ) ++legCnt;
    }

    pLeg = pLeg0;// restore initial assignment
}

/*
void makeSpiralPath( std::ifstream& inFile, Leg*& pLeg )
{
    float X0, Y0, par0, parF;
    inFile >> X0 >> Y0 >> par0 >> parF;
    float r0, rF;
    inFile >> r0 >> rF;
    size_t NumSegs;
    inFile >> NumSegs;
    float k = ( rF - r0 )/( parF - par0 );
    pLeg = new funcLeg( X0, Y0, par0, parF, 'p', [k,r0,par0]( float a ){ return r0 + k*( a - par0 ); }, [k]( float a ){ (void)a; return k; }, NumSegs );
  //  pLeg->prev = pLeg->next = nullptr;
    new linLeg( *pLeg, *pLeg );
}   */

void makeFuncPath( std::ifstream& inFile, Leg*& pLeg )
{
    float x0, y0, x1, y1;//, angle;
    char legType = 'n';// 'L'=linLeg, 'C'=cirLeg
    // 1st Leg takes full params
    inFile >> legType;
    if( legType == 'L' )
    {
        inFile >> x0 >> y0 >> x1 >> y1;
        pLeg = new linLeg( x0, y0, x1, y1 );// 1st leg
    }
    else if( legType == 'S' )// spiral
    {
        inFile >> x0 >> y0 >> x1 >> y1;// x1, y1 = par0, parF
        float r0, rF;
        inFile >> r0 >> rF;
        size_t NumSegs;
        inFile >> NumSegs;
        float k = ( rF - r0 )/( y1 - x1 );
        pLeg = new funcLeg( x0, y0, x1, y1, 'p', [k,r0,x1]( float a ){ return r0 + k*( a - x1 ); }, [k]( float a ){ (void)a; return k; }, NumSegs );
    }
    else if( legType == 'K' )// cubic
    {
        inFile >> x0 >> y0 >> x1 >> y1;// x1, y1 = par0, parF
        float c;
        inFile >> c;
        size_t NumSegs;
        inFile >> NumSegs;
        pLeg = new funcLeg( x0, y0, x1, y1, 'c', [c]( float x ){ return c*x*x*x; }, [c]( float x ){ return 3.0f*c*x*x; }, NumSegs );
    }
    else if( legType == 'H' )// heart = cardiod
    {
        inFile >> x0 >> y0 >> x1 >> y1;// x1, y1 = par0, parF
        float c, a0;
        inFile >> c >> a0;
        size_t NumSegs;
        inFile >> NumSegs;
        pLeg = new funcLeg( x0, y0, x1, y1, 'p', [c,a0]( float a ){ return c*( 1.0f + cosf(a-a0) ); }, [c,a0]( float a ){ return c*sinf(a-a0); }, NumSegs );
    }
    else if( legType == 'T' )// trig = sine wave
    {
        inFile >> x0 >> y0 >> x1 >> y1;// x1, y1 = par0, parF = 0, 2pi
        float Amp, wvLen;
        inFile >> Amp >> wvLen;
        wvLen /= 6.2832f;
        size_t NumSegs;
        inFile >> NumSegs;
        pLeg = new funcLeg( x0, y0, x1*wvLen, y1*wvLen, 'c', [Amp,wvLen]( float x ){ return Amp*sinf(x/wvLen); }, [Amp,wvLen]( float x ){ return Amp*cosf(x/wvLen)/wvLen; }, NumSegs );
    }
    else if( legType == 'E' )// ellipse
    {
        float a, b;
        inFile >> x0 >> y0 >> a >> b;// x1, y1 = par0, parF = 0, 2pi
        size_t NumSegs;
        inFile >> NumSegs;
        pLeg = new funcLeg( x0, y0, -a*0.999f, a*0.999f, 'c', [a,b](float x){ return b*sqrtf( 1 - x*x/(a*a) ); }, [a,b](float x){ return -b*x/( a*a*sqrtf( 1 - x*x/(a*a) ) ); }, NumSegs );
    }
    else if( legType == 'e' )// ellipse
    {
        float a, b;
        inFile >> x0 >> y0 >> a >> b;// x1, y1 = par0, parF = 0, 2pi
        float a1, a2;
        inFile >> a1 >> a2;
        float k = 1.0f - b*b/(a*a);
        size_t NumSegs;
        inFile >> NumSegs;

    //    auto f = [b,k](float x){ return -b*k*cosf(x)*sinf(x)*powf( 1.0f - k*cosf(x)*cosf(x), -1.5f ); };
        std::function<float(float)> f = [b,k](float x){ return -b*k*cosf(x)*sinf(x)*powf( 1.0f - k*cosf(x)*cosf(x), -1.5f ); };
        pLeg = new funcLeg( x0, y0, a1, a2, 'p', [b,k](float x){ return b/sqrtf( 1.0 - k*cosf(x)*cosf(x) ); }, f, NumSegs );
    }

 //   pLeg->next = pLeg->prev = nullptr;
    Leg* it = pLeg;

    while( inFile >> legType )
    {
        if( legType == 'X' )// path closer
        {
            new linLeg( *it, *pLeg );
            return;
        }
        if( legType == 'J' )// just join 1st and last
        {
            it->next = pLeg;
            pLeg->prev = it;
            return;
        }
        // mid legs
        if( legType == 'L' )
        {
            inFile >> x1 >> y1;
            new linLeg( *it, x1, y1 );// 1st leg
        }
        else if( legType == 'S' )// spiral
        {
            inFile >> x1 >> y1;// x1, y1 = par0, parF
            float r0, rF;
            inFile >> r0 >> rF;
            size_t NumSegs;
            inFile >> NumSegs;
            float k = ( rF - r0 )/( y1 - x1 );
            new funcLeg( *it, x1, y1, 'p', [k,r0,x1]( float a ){ return r0 + k*( a - x1 ); }, [k]( float a ){ (void)a; return k; }, NumSegs );
        }
        else if( legType == 'K' )// cubic
        {
            inFile >> x1 >> y1;// x1, y1 = par0, parF
            float c;
            inFile >> c;
            size_t NumSegs;
            inFile >> NumSegs;
            new funcLeg( *it, x1, y1, 'c', [c]( float x ){ return c*x*x*x; }, [c]( float x ){ return 3.0f*c*x*x; }, NumSegs );
        }
        else if( legType == 'H' )// heart = cardiod
        {
            inFile >> x1 >> y1;// x1, y1 = par0, parF
            float c, a0;
            inFile >> c >> a0;
            size_t NumSegs;
            inFile >> NumSegs;
            new funcLeg( *it, x1, y1, 'p', [c,a0]( float a ){ return c*( 1.0f + cosf(a-a0) ); }, [c,a0]( float a ){ return c*sinf(a-a0); }, NumSegs );
        }
        else if( legType == 'T' )// trig = sine wave
        {
            inFile >> x1 >> y1;// x1, y1 = par0, parF = 0, 2pi
            float Amp, wvLen;
            inFile >> Amp >> wvLen;
            wvLen /= 6.2832f;
            size_t NumSegs;
            inFile >> NumSegs;
            new funcLeg( *it, x1*wvLen, y1*wvLen, 'c', [Amp,wvLen]( float x ){ return Amp*sinf(x/wvLen); }, [Amp,wvLen]( float x ){ return Amp*cosf(x/wvLen)/wvLen; }, NumSegs );
        }
        else if( legType == 'E' )// ellipse
        {
            float a, b;
            inFile >> a >> b;// x1, y1 = par0, parF = 0, 2pi
            size_t NumSegs;
            inFile >> NumSegs;
            new funcLeg( *it, -a*0.999f, a*0.999f, 'c', [a,b](float x){ return b*sqrtf( 1 - x*x/(a*a) ); }, [a,b](float x){ return -b*x/( a*a*sqrtf( 1 - x*x/(a*a) ) ); }, NumSegs );
        }
        else if( legType == 'e' )// ellipse
    {
        float a, b;
        inFile >> a >> b;// x1, y1 = par0, parF = 0, 2pi
        float a1, a2;
        inFile >> a1 >> a2;
        float k = 1.0f - b*b/(a*a);
        size_t NumSegs;
        inFile >> NumSegs;

        auto f = [b,k](float x){ return -b*k*cosf(x)*sinf(x)*powf( 1.0f - k*cosf(x)*cosf(x), -1.5f ); };
        pLeg = new funcLeg( *it, a1, a2, 'p', [b,k](float x){ return b/sqrtf( 1.0 - k*cosf(x)*cosf(x) ); }, f, NumSegs );
    }

        it = it->next;
    }

}

void makeFlyPath_lin( std::ifstream& fin, Leg*& pLeg )
{
    float lenMin, lenMax;
    fin >> lenMin >> lenMax;
    float xMin, xMax, yMin, yMax;
    fin >> xMin >> xMax >> yMin >> yMax;

    size_t N_legs = 0;
    fin >> N_legs;
    float x0, y0;
    fin >> x0 >> y0;// entry leg
    float xi, yi;
    fin >> xi >> yi;
    pLeg = new linLeg(x0,y0,xi,yi);

    std::default_random_engine rand_gen;
    std::uniform_real_distribution<float> len_dist(lenMin,lenMax);
    std::uniform_real_distribution<float> dir_dist( -3.1416f, 3.1416f );

    Leg* pLeg_it = pLeg;
    for( size_t i=0; i<N_legs; ++i )
    {
        float xf, yf;
        float ang = dir_dist(rand_gen);
        float len = len_dist(rand_gen);
        xf = xi + len*cosf(ang);
        yf = yi + len*sinf(ang);

        if( xf < xMin ) xf = xMin;
        else if( xf > xMax ) xf = xMax;
        if( yf < yMin ) yf = yMin;
        else if( yf > yMax ) yf = yMax;

        if( pLeg_it )
        {
            new linLeg( *pLeg_it, xf, yf );
            pLeg_it = pLeg_it->next;
        }
        else break;

        xi = xf;
        yi = yf;
    }

    // final connectin leg
    new linLeg( *pLeg_it, *pLeg->next );
}

void makeFlyPath_cir( std::ifstream& fin, Leg*& pLeg )
{
    float rMin, rMax;
    fin >> rMin >> rMax;
    float xMin, xMax, yMin, yMax;// will apply to center position
    fin >> xMin >> xMax >> yMin >> yMax;

    size_t N_legs = 0;
    fin >> N_legs;
    float x0, y0;
    fin >> x0 >> y0;// entry leg
    float xi, yi;
    fin >> xi >> yi;
    pLeg = new linLeg(x0,y0,xi,yi);

    std::default_random_engine rand_gen;
    std::uniform_real_distribution<float> len_dist(rMin,rMax);
    std::uniform_real_distribution<float> ang_dist( -3.1416f, 3.1416f );

    Leg* pLeg_it = pLeg;
    for( size_t i=0; i<N_legs; ++i )
    {
        float xc, yc;
        float ang = ang_dist(rand_gen);
        float R = len_dist(rand_gen);
        float Nux, Nuy;
        pLeg->T( pLeg->len, Nuy, Nux );
        if( ang < 0.0f )// ccw motion. Use LH norm to find center
        {
         //   Nux *= -1.0f;// orig
            Nuy *= -1.0f;
        }
        else// cw motion. Use RH norm
        {
         //   Nuy *= -1.0f;// orig
            Nux *= -1.0f;
        }

        xc = xi + R*Nux;
        yc = yi + R*Nuy;

    //    if( xf < xMin ) xf = xMin;
    //    else if( xf > xMax ) xf = xMax;
     //   if( yf < yMin ) yf = yMin;
     //   else if( yf > yMax ) yf = yMax;

        if( pLeg_it )
        {
            new cirLeg( *pLeg_it, xc, yc, ang );
            pLeg_it = pLeg_it->next;
            if( pLeg_it ) { xi = pLeg_it->x(pLeg_it->len); yi = pLeg_it->y(pLeg_it->len);
 }
        }
        else break;

    //    xi = xf;
    //    yi = yf;
    }

    // final connectin leg
    new linLeg( *pLeg_it, *pLeg->next );
}

void makeBlockPath( std::ifstream& fin, std::vector<Leg*>& pLegVec )
{
    float ye, y0, dy, xe, xLt, xRt;
    fin >> ye >> y0 >> dy >> xe >> xLt >> xRt;
    size_t rows = 0; fin >> rows;
    float y = y0 + 2.0f*dy*(rows-1);
    for( size_t i=0; i<rows; ++i )
    {
        Leg* pE = new linLeg( xe, ye, xe, y );// entry leg
        Leg* it = pE;// entry leg
        pLegVec.push_back( pE );
        it->next = new linLeg( *it, xRt, y ); it = it->next;// top - rt half
        it->next = new linLeg( *it, xRt, y + dy ); it = it->next;// rt side
        it->next = new linLeg( *it, xLt, y + dy ); it = it->next;// bottom
        it->next = new linLeg( *it, xLt, y ); it = it->next;// lt side
     //   it->next = new linLeg( *it, xe, y ); it = it->next;// top - lt half
        it->next = new linLeg( *it, *pE->next );// top - lt half closes path
   //     it->next = pE->next;// closing path
        y -= 2.0f*dy;
    }
}

int destroyPath( Leg*& pLeg )
{
    if( !pLeg ) return 0;

    int nLegs = 0;
    Leg* pLeg0 = pLeg;// closed path watch value

    // entryLeg ?
    if( pLeg->prev == nullptr )
    {
        pLeg0 = pLeg->next;
        delete pLeg;// entryLeg gone
        pLeg = pLeg0;// onto rest of path
        nLegs = 1;
    }

    if( pLeg == nullptr ) return 1;// single open leg

    do
    {
        Leg* temp = pLeg;
        pLeg = pLeg->next;
        delete temp;
        ++nLegs;
    }while( pLeg && pLeg != pLeg0 );// good for dead end paths too

    pLeg = nullptr;
    return nLegs;
}

void tracePath( Leg& rLeg, float ds, std::vector<sf::Vertex>& vtxVec, sf::Color reg, sf::Color join )
{

    const Leg *pLeg0 = &rLeg;
    const Leg *it = pLeg0, *itLast = nullptr;
    float s = 0.0f;
    sf::Vertex v;
    v.position = sf::Vector2f( it->x(s), it->y(s) );
    v.color = join;
    vtxVec.push_back(v);
    int Nvtx = 0;

    if( rLeg.prev == nullptr )
    {
        do
        {
            it = it->update( v.position.x, v.position.y, s, ds, 1.0f );
            v.color = sf::Color::Blue;
            vtxVec.push_back(v);
            ++Nvtx;
        } while( it == pLeg0 );
        pLeg0 = pLeg0->next;
    }

    if( !it ) return;// one legged path

    while( Nvtx < 2000 )
    {
    //    sf::Vertex v;
        it = it->update( v.position.x, v.position.y, s, ds, 1.0f );
        v.color = reg;
        if( !it ) break;// dead end path

        if( it != itLast )// leg change
        {
            s = 0.0f;
            v.position.x = it->x(s);
            v.position.y = it->y(s);
            v.color = join;
            if(it == pLeg0 && itLast == pLeg0->prev) break;
            itLast = it;
        }
        vtxVec.push_back(v);
        ++Nvtx;
    }
    std::cout << "\npathUtils::tracePath: Nvtx = " << Nvtx;
}

float pathLength( Leg& rLeg )
{
    Leg* pStLeg = &rLeg;
    if( !rLeg.prev ) pStLeg = rLeg.next;// skip entry Leg
    Leg* iter = pStLeg;
    if( !iter ) return 0.0f;
    float len = 0.0f;

    do
    {
        len += iter->len;
        iter = iter->next;
    } while( iter && (iter != pStLeg) );

    return len;
}
