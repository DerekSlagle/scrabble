#include "BkgdArr.h"

sf::Rect<float> BkgdArr::rGR;

void BkgdArr::init( const sf::Texture& rTxt, float Scale, std::ifstream& fin )
{
    float posY = 0.0f;
    fin >> vFactor >> posY >> nEle;

    pEle.clear();
    pEle.resize(nEle);
    vx = 0.0f;
    idxLo = 0;

    int l, r, t, b;
    float Sep_x, offY, x = 0.0f;
    for(int j=0; j< nEle; ++j)
    {
        fin >> l >> r >> t >> b >> Sep_x >> offY;
        float y = rGR.top + rGR.height - posY*Scale + offY;
        pEle[j].INIT( rTxt, l, r, t, b, Sep_x, x, y, Scale );
    //    std::cout << "\nx = " << x << " y = " << y << " sepX = " << Sep_x;

        if( x < rGR.left + rGR.width ) idxHi = j;
        x += pEle[j].spr.getLocalBounds().width + pEle[j].sepX;
    }
}

void BkgdArr::update( float dt )
{
    if( vx < 0.0f )// moving right  to left
    {
        // move off left end of window
        if(( pEle[idxLo].spr.getPosition().x + pEle[idxLo].spr.getLocalBounds().width ) < rGR.left ) idxLo = (1+idxLo)%nEle;
        // add element to right end
        if(( pEle[idxHi].spr.getPosition().x + pEle[idxHi].spr.getLocalBounds().width + pEle[idxHi].sepX ) <= rGR.left + rGR.width )
        {
            idxHi = (1+idxHi)%nEle;
            pEle[idxHi].spr.setPosition( rGR.left + rGR.width + vx, pEle[idxHi].spr.getPosition().y );
        }
    }
    else if( vx > 0.0f )
    {
        // move off right end of window
        if(( pEle[idxHi].spr.getPosition().x ) > rGR.left + rGR.width ) { --idxHi; if( idxHi < 0 ) idxHi = nEle - 1; }
        // add element to left end
      //  if(( pEle[idxLo].spr.getPosition().x - pEle[idxLo].spr.getLocalBounds().width ) > rGR.left )
        if(( pEle[idxLo].spr.getPosition().x ) > rGR.left )
        {
            --idxLo; if( idxLo < 0 ) idxLo = nEle - 1;
         //   pEle[idxLo].spr.setPosition( rGR.left - vx - pEle[idxLo].sepX, pEle[idxLo].spr.getPosition().y );
            pEle[idxLo].spr.setPosition( rGR.left - pEle[idxLo].spr.getLocalBounds().width - pEle[idxLo].sepX, pEle[idxLo].spr.getPosition().y );
        }
    }

    int idx = idxLo;

    do// move visible elements
    {
        pEle[idx].spr.move( vx, 0.0f );
        if( idx == idxHi ) break;
        ++idx;
        if( idx == nEle ) idx = 0;

    }while( true );

    return;
}

/*
void BkgdArr::update( float dt )
{
    int idx = idxLo;

    do// move visible elements
    {
        pEle[idx].spr.move( vx, 0.0f );
        if( vx < 0.0f )
        {
            if(( pEle[idx].spr.getPosition().x + pEle[idx].spr.getLocalBounds().width ) < rGR.left )
            {
                idxLo = (1+idxLo)%nEle;// move off left end of window
            //    cout << "move(): idxLo = " << idxLo << endl;
            }
        }
        if( idx == idxHi ) break;
        ++idx;
        if( idx == nEle ) idx = 0;

    }while( true );

    // add element to right end
    if(( pEle[idxHi].spr.getPosition().x + pEle[idxHi].spr.getLocalBounds().width + pEle[idxHi].sepX ) <= rGR.left + rGR.width )
    {
        idxHi = (1+idxHi)%nEle;
//        pEle[idxHi].spr.SetX( rGR.Right );
        pEle[idxHi].spr.setPosition( rGR.left + rGR.width + vx, pEle[idxHi].spr.getPosition().y );
    //    cout << "move(): idxHi = " << idxHi << endl;
    }

    return;
}
*/

void BkgdArr::draw( sf::RenderTarget& RT ) const
{
    int idx = idxLo;

    do// move visible elements
    {
        RT.draw( pEle[idx].spr );
        if( idx == idxHi ) break;
        ++idx;
        if( idx == nEle ) idx = 0;

    }while( true );

    return;
}

void BkgdArr::setColor( const sf::Color& newColor )// used to fade out a bkgd layer
{
    for( int i=0; i<nEle; ++i)
        pEle[i].spr.setColor( newColor );
    return;
}

/*
void BkgdArr::draw( sf::RenderWindow& rApp )
{
    for(int i=idxLo; i != idxHi+1; ++i)
    {
        if( i == nEle ) i = 0;
        rApp.Draw( pEle[i].spr );
    }

    return;
}


void BkgdArr::move(void)
{
    for(int i=0; i<nEle; ++i)
    {
        pEle[i].spr.Move( vx, vy );

        if( vx < 0.0f && ( pEle[i].spr.GetPosition().x + pEle[i].spr.GetSize().x ) < rGR.Left )
        {
            pEle[i].spr.SetX( rGR.Right );
            continue;
        }

        if( vx > 0.0f && pEle[i].spr.GetPosition().x > rGR.Right )
        {
            pEle[i].spr.SetX( rGR.Left );
            continue;
        }
    }
    return;
}

void BkgdArr::draw( sf::RenderWindow& rApp )
{
    for(int i=0; i<nEle; ++i)
    {
        rApp.Draw( pEle[i].spr );
    }

    return;
}

*/
