#include "dialDisplay.h"

const float RtoD = 180.0f/3.1416f;

bool dialDisplay::init( sf::Vector2f Pos, const char* fName )
{
    if( !(pDialTxt && pNeedleTxt) ) return false;
    std::ifstream fin( fName );
    if( !fin ) { std::cout << "\nno file = " << fName; return false; }
    std::string imageFname;
    fin >> imageFname;// dial
    if( !pDialTxt->loadFromFile( imageFname.c_str() ) ) { std::cout << "\nno dialTxt = " << imageFname; return false; }
    fin >> imageFname;// needle
    if( !pNeedleTxt->loadFromFile( imageFname.c_str() ) ) { std::cout << "\nno needleTxt = " << imageFname; return false; }

    sf::IntRect ndlSrcRect;
    fin >> ndlSrcRect.left >> ndlSrcRect.top >> ndlSrcRect.width >> ndlSrcRect.height;
    float endOfst, dialScale, needleScale;
    fin >> endOfst >> dialScale >> needleScale;
    fin >> needleOfst.x >> needleOfst.y; needleOfst *= dialScale;
    fin >> minVal >> maxVal >> minAngle >> maxAngle;
    currVal = 0.0f;

    fin >> labelOfst.x >> labelOfst.y;
    label.setPosition( Pos + labelOfst );

    dialSpr.setTexture( *pDialTxt );
    dialSpr.setScale( dialScale, dialScale );
    dialSpr.setPosition( Pos );
    pos = Pos;
    needleSpr.setTexture( *pNeedleTxt );
    needleSpr.setTextureRect( ndlSrcRect );
    needleSpr.setScale( needleScale, needleScale );
    needleSpr.setOrigin( ndlSrcRect.width*needleScale/2.0f, endOfst );
    needleSpr.setPosition( pos + needleOfst );
    needleSpr.setRotation( minAngle );
    setValue( currVal );
}

void dialDisplay::setPosition( sf::Vector2f Pos )
{
    dialSpr.setPosition( Pos );
    needleSpr.setPosition( Pos + needleOfst );
    label.setPosition( Pos + labelOfst );
    pos = Pos;
}

void dialDisplay::update( float dt )
{
    (void)dt;
}

void dialDisplay::setValue( float val )
{
    currVal = val;
    float angRange = maxAngle - minAngle;
    float valRange = maxVal - minVal;
    float angle = minAngle + angRange*(val-minVal)/valRange;
    if( angle < minAngle ) { angle = minAngle; currVal = minVal; }
    else if( angle > maxAngle ) { angle = maxAngle; currVal = maxVal; }
    needleSpr.setRotation( angle );
}
