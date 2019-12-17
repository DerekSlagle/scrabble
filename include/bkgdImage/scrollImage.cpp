#include "scrollImage.h"

sf::Rect<float> scrollImage::rGR;//.left = 0.0f, scrollImage::rGR.top = 0.0f;
//sf::Rect<float> scrollImage::rGR.width = 800.0f, scrollImage::rGR.height = 600.0f;

scrollImage::scrollImage()
{
    //ctor
}

scrollImage::scrollImage( const sf::Texture& rTxt, std::ifstream& fin )
{
    int l, r, t, b;
    float v_Factor = 0.0f, posY = 0.0f;

    fin >> v_Factor >> posY >> l >> r >> t >> b;

    INIT( rTxt, l, r, t, b, v_Factor, posY );
}

scrollImage::~scrollImage()
{
    //dtor
}

bool scrollImage::INIT( const sf::Texture& rTxt, int left, int right, int top, int bottom, float v_Factor, float Elev )
{
    src.left = left;
    src.width = right - left;
    src.top = top;
    src.height = bottom - top;

    imgSzX = src.width;
    imgSzY = src.height;

    sprL.setTexture( rTxt );
    sprR.setTexture( rTxt );
    x_left = (float)src.left;
    vx = 0.0f;
    vFactor = v_Factor;
    drawRight = false;

    elev = Elev;

    float yTop = rGR.height - (float)( bottom - top ) - elev;
    sprL.setPosition( rGR.left, yTop );
    sprR.setPosition( sprR.getPosition().x, yTop );
 //   sprR.setY( yTop );

 //   vFactor = v_Factor;// new

    x0 = (int)rGR.left;
    y0 = (int)rGR.top;
    W = (int)(rGR.width);
    H = (int)(rGR.height);

    move();

    return true;
}

int scrollImage::move(void)
{
 //   int W = (int)(rGR.Right - rGR.Left);// replaced by dm

    x_left -= vx;
    int xL = (int)x_left;
    if( xL > imgSzX + src.left )
    {
        x_left -= (float)imgSzX;
        xL -= imgSzX;
    }
    else if( xL < src.left )
    {
        x_left += (float)imgSzX;
        xL += imgSzX;
    }

    if( xL + W > imgSzX )
        drawRight = true;
    else
        drawRight = false;

    sf::Rect<int> srcRect;
    srcRect.top = src.top;
    srcRect.height = src.height;
    if( drawRight )
    {
        srcRect.left = xL + src.left;
        srcRect.width = imgSzX;
        sprL.setTextureRect( srcRect );

        srcRect.left = src.left;
        srcRect.width = W + xL - imgSzX;
        sprR.setTextureRect( srcRect );
        sprR.setPosition( rGR.left + (float)( imgSzX - xL ), sprR.getPosition().y );
    }
    else
    {
        srcRect.left = xL + src.left;
        srcRect.width = xL + W;
        sprL.setTextureRect( srcRect );
    }
    return xL;
}// end of move()

void scrollImage::draw( sf::RenderTarget& RT ) const
{
    RT.draw( sprL );
    if( drawRight )
        RT.draw( sprR );
    return;
}

void scrollImage::setColor( const sf::Color& newColor )// used to fade out the scrolling background
{
    sprL.setColor( newColor );
    sprR.setColor( newColor );

    return;
}

// new - for vertical scrolling
/*
void scrollImage::INIT_vert( const sf::Texture& rTxt, sf::Rect<int> src_rect, float X0, float Y0, int width, int height, float v, float scroll_0 )
{
    src.Left = src_rect.Left;
    src.Right = src_rect.Right;
    src.Top = src_rect.Top;
    src.Bottom = src_rect.Bottom;

    imgSzX = src.Right - src.Left;
    imgSzY = src.Bottom - src.Top;

    sprL.SetImage( rImg );
    sprR.SetImage( rImg );
    x_left = scroll_0;
    vx = v;
    drawRight = false;

    elev = 0.0f;// unused for vertical scrolling

    x0 = X0*g_scale;
    y0 = Y0*g_scale;
    H = height;// MUST be < imgSzY
    if( width <= imgSzX )
        W = width;
    else
        W = imgSzX;

    W = (int)(W*g_scale);
    H = (int)(H*g_scale);

    sprL.SetPosition( x0, y0 );
    sprR.SetX( x0 );

    vFactor = 1.0f;

    move_vert();

    return;
}

void scrollImage::move_vert(void)
{
    x_left -= vx;
    int xL = (int)x_left;
    if( xL > imgSzY )
    {
        x_left -= (float)imgSzY;
        xL -= imgSzY;
    }
    else if( xL < 0 )
    {
        x_left += (float)imgSzY;
        xL += imgSzY;
    }

    if( xL + H > imgSzY )
        drawRight = true;
    else
        drawRight = false;

    sf::Rect<int> srcRect;
    srcRect.Left = src.Left;
    srcRect.Right = src.Left + W;//src.Right;
    if( drawRight )
    {
        srcRect.Top = xL + src.Top;
        srcRect.Bottom = src.Bottom;
        sprL.SetSubRect( srcRect );

        srcRect.Top = src.Top;
        srcRect.Bottom = H + xL - imgSzY + src.Top;
        sprR.SetSubRect( srcRect );
        sprR.SetY( y0 + (float)( imgSzY - xL ) );
    }
    else
    {
        srcRect.Top = xL + src.Top;
        srcRect.Bottom = xL + H + src.Top;
        sprL.SetSubRect( srcRect );
    }
    return;
}   */

/*
void scrollImage::move_vert(void)
{
    x_left -= vx;
    int xL = (int)x_left;
    if( xL > imgSzY + src.Top )
    {
        x_left -= (float)imgSzY;
        xL -= imgSzY;
    }
    else if( xL < src.Top )
    {
        x_left += (float)imgSzY;
        xL += imgSzY;
    }

    if( xL + H > imgSzY )
        drawRight = true;
    else
        drawRight = false;

    sf::Rect<int> srcRect;
    srcRect.Left = src.Left;
    srcRect.Right = src.Right;
    if( drawRight )
    {
        srcRect.Top = xL + src.Top;
        srcRect.Bottom = imgSzY + src.Top;
        sprL.SetSubRect( srcRect );

        srcRect.Top = src.Top;
        srcRect.Bottom = H + xL - imgSzY + src.Top;
        sprR.SetSubRect( srcRect );
        sprR.SetY( y0 + (float)( imgSzY - xL ) );
    }
    else
    {
        srcRect.Top = xL + src.Top;
        srcRect.Bottom = xL + H + src.Top;
        sprL.SetSubRect( srcRect );
    }
    return;
}
*/
