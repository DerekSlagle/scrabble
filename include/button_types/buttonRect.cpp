#include "buttonRect.h"

sf::Color buttonRect::selClr = sf::Color::White, buttonRect::unSelClr = sf::Color(128,128,128);
sf::Color buttonRect::moClr = sf::Color::Green, buttonRect::unMoClr = sf::Color::Blue;

buttonRect::buttonRect(): sz(10.0f,10.0f)
{
    //ctor
}

buttonRect::~buttonRect()
{
    //dtor
}

bool buttonRect::hit()const
{
    if( mseX < pos.x || mseX > pos.x+sz.x ) return false;
    if( mseY < pos.y || mseY > pos.y+sz.y ) return false;
    return true;
}

void buttonRect::draw( sf::RenderTarget& rRW )const
{
    size_t idx = 0;
    if( sel ) idx = 2;
    if( mseOver ) ++idx;

 //   R.setFillColor( (sel ? selClr : unSelClr) );
 //   R.setFillColor( sf::Color::Red );
 //   R.setOutlineColor( (mseOver ? moClr : unMoClr) );
//    R.setOutlineColor( sf::Color::Yellow );

    rRW.draw( R );
    rRW.draw(title);
}

void buttonRect::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pos = Pos;
    title.setPosition( title.getPosition() + dPos );

    R.setPosition(pos);
}

bool buttonRect::MseOver()
{
 //   bool moOld = mseOver;
    button::MseOver();
 //   if( mseOver != moOld ) R.setOutlineColor( (mseOver ? moClr : unMoClr ) );
    R.setOutlineColor( (mseOver ? moClr : unMoClr ) );
    return mseOver;
}

bool buttonRect::hitLeft()
{
    if( mode == 'T' && !button::mseDnLt ) return false;
    bool selOld = sel;
    button::hitLeft();
    if( sel != selOld ) R.setFillColor( (sel ? selClr : unSelClr ) );
    return mseOver;
}

void buttonRect::setSel( bool Sel )
{
    button::setSel( Sel );
    R.setFillColor( (sel ? selClr : unSelClr ) );
}

/*
void buttonRect::setPosition( sf::Vector2f Pos )
{
    pos = Pos;
    sf::Vector2f dPos;
    sf::FloatRect titleSz = title.getLocalBounds();
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
        R[i].setPosition(pos);
}
*/

void buttonRect::init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const char* label )
{
    sf::Text Title( label, *button::pFont, 12 );
    Title.setColor( sf::Color::Black );
    init( x, y, W, H, p_HitFunc, Title );
}

void buttonRect::init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const sf::Text& Title )
{
    pos.x = x;
    pos.y = y;
    sz.x = W; sz.y = H;
    pHitFunc = p_HitFunc;

    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    dPos += pos;// new
    dPos.x = floor( dPos.x );// new
    dPos.y = floor( dPos.y - 2.0f );// new
    title.setPosition(dPos);

    R.setSize( sf::Vector2f(W,H) );
    R.setPosition( pos );
    R.setOutlineThickness( -3.0f );
    R.setFillColor( (sel ? selClr : unSelClr) );
    R.setOutlineColor( (mseOver ? moClr : unMoClr) );

 //   sf::RectangleShape pR[4];
 //   initButtRects( pR, W, H );
 //   buttonRect::init( pR, x, y, Title );
}

/*
void buttonRect::init( const sf::RectangleShape* pR, float x, float y, const sf::Text& Title )
{

    pos.x = x;
    pos.y = y;
    sz = pR->getSize();

    title = Title;
    sf::FloatRect titleSz = title.getLocalBounds();
    sf::Vector2f dPos;
    dPos.x = (sz.x - titleSz.width)/2.0f;
    dPos.y = (sz.y - titleSz.height)/2.0f;
    title.setPosition(pos+dPos);

    for( size_t i=0; i<4; ++i )
    {
        R[i] = pR[i];
        R[i].setPosition(pos);
    }
}

// helpers
void initButtRects( sf::RectangleShape* R, float W, float H, sf::Vector2f Pos, sf::Color unSelClr, sf::Color unSelMoClr, sf::Color selClr, sf::Color selMoClr )
{
    for( size_t i=0; i<4; ++i )
    {
        R[i].setSize( sf::Vector2f(W,H) );
        R[i].setPosition( Pos );
        R[i].setOutlineThickness( -3.0f );
    }

    // unsel
    R[0].setFillColor( unSelClr );
    R[0].setOutlineColor( unSelMoClr );
    // unsel - mo
    R[1].setFillColor( unSelClr );
    R[1].setOutlineColor( selMoClr );

    // sel
    R[2].setFillColor( selClr );
    R[2].setOutlineColor( unSelMoClr );
    // sel - mo
    R[3].setFillColor( selClr );
    R[3].setOutlineColor( selMoClr );
}

void initButtRects( sf::RectangleShape* R, float W, float H )
{
    initButtRects( R, W, H, sf::Vector2f(0.0f,0.0f), sf::Color(200,200,200), sf::Color(0,0,255), sf::Color(255, 255, 255), sf::Color(0, 255, 0) );
}
*/
