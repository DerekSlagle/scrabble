#include "floatSelector.h"

void floatSelector::scrollTune()
{
    if( button::didScroll && !sel )// fine tune value while mouse over
    {
        float newVal = fVal + tuneFactor*( fVal_max - fVal_min)*button::scrollAmount;
        set_fVal( newVal );
    }
}

void multiSelector::init( sf::Vector2f pos, floatSelector* p_FS, std::istream& is )
{
    pFS = p_FS;
    mseOver = false;
    sel = true;

    float stripLen, stripHgt; is >> stripLen >> stripHgt;
    float buttW = 50.0f, buttH = 25.0f, spcX = 10.0f;
    is >> buttW >> buttH >> spcX;
    sf::Text label("", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    sf::Vector2f ofst;// is >> ofst.x >> ofst.y;

    size_t Nbutts; is >> Nbutts;
    buttonRect tmpRect;
    stripData tmpSD;
    std::string labelStr_butt, labelStr_strip;
    sdataVec.reserve(Nbutts);
    selButtVec.reserve(Nbutts);
    float minVal, maxVal, iVal;
    float buttPosX = pos.x, buttPosY = pos.y;
    // for each button
    for( size_t i = 0; i < Nbutts; ++i )
    {
        is >> labelStr_butt >> labelStr_strip;
        label.setString( labelStr_butt.c_str() );
        is >> minVal >> maxVal >> iVal;
        tmpSD.init( labelStr_strip, minVal, maxVal, iVal, nullptr );// assigned in lvl_SparkAni
        sdataVec.push_back( tmpSD );
        if( buttPosX + buttW - pos.x > stripLen + 50.0f ){ buttPosX = pos.x; buttPosY += buttH + spcX; }
        tmpRect.init( buttPosX, buttPosY, buttW, buttH, nullptr, label );// ptr requrs * to stripData
        buttPosX += buttW + spcX;
        selButtVec.push_back( tmpRect );
     //   Csurf.pButtVec.push_back( &selButtVec.back() );
    }

    is >> ofst.x >> ofst.y;
    if( !sdataVec.empty() )
    {
        stripData* pSD = &sdataVec[0];
        label.setString( pSD->label.c_str() );
        pFS->init( pos.x+ofst.x, buttPosY+buttH+ofst.y, stripLen, stripHgt, label, pSD->xMin, pSD->xMax, pSD->xCurr, pSD->pSetFunc );
    //    Csurf.pButtVec.push_back( &theStrip );
    }

    if( !selButtVec.empty() ){ idx_curr = 0; selButtVec[0].setSel(true); }

 /*   std::function<void(buttonRect*,stripData*)> pFbutt = [this]( buttonRect* pBRnow, stripData* pSD )
    {
    //    if( pBR_curr ) pBR_curr->setSel(false); pBR_curr = pBRnow;
    //    pFS->reInit( *pSD );
    };

    buttonRect* pBR = &selButtVec[0];
    stripData* pSD = &sdataVec[0];
    size_t iHi = selButtVec.size();
    for( size_t i = 0; i < iHi; ++i )
    {
        pBR->pHitFunc = [this,pFbutt,pSD,pBR](){ pFbutt(pBR,pSD); };
        if( i+1 == iHi ) break;
        ++pBR; ++pSD;
    }   */
}

bool multiSelector::getValue( const std::string& Label, float& v )const// writes to v and returns true if idx in range
{
    for( unsigned int i = 0; i < sdataVec.size(); ++i )
    if( sdataVec[i].label == Label ) return getValue( i, v );

    return false;
}

bool multiSelector::setValue( const std::string& Label, float v )// writes to sdataVec[idx] and calls set func and returns true if idx in range
{
    for( unsigned int i = 0; i < sdataVec.size(); ++i )
    if( sdataVec[i].label == Label ) return setValue( i, v );

    return false;
}

bool multiSelector::getValue( unsigned int idx, float& v )const// writes to v and returns true if idx in range
{
    if( idx < sdataVec.size() )
    {
        v = sdataVec[idx].xCurr;
        return true;
    }
    return false;
}

bool multiSelector::setValue( unsigned int idx, float v )// writes to sdataVec[idx] and calls set func and returns true if idx in range
{
    if( idx < sdataVec.size() )
    {
        sdataVec[idx].xCurr = v;
        if( idx == idx_curr ) pFS->set_fVal(v);
        else if( sdataVec[idx].pSetFunc ) sdataVec[idx].pSetFunc(v);
        return true;
    }
    return false;
}

bool multiSelector::hit()const
{
    if( !sel ) return false;
    if( pFS->hit() ) return true;
    for( auto& butt : selButtVec ) if( butt.hit() ) return true;
    return false;
}

void multiSelector::draw( sf::RenderTarget& RT )const
{
    if( !sel ) return;
    for( auto& butt : selButtVec ) butt.draw(RT);
    pFS->draw(RT);
}

bool multiSelector::MseOver()
{
    if( !sel ) { mseOver = false; return false; }
 //   if( pFS->MseOver() ) { mseOver = true; idx_mse = -1; return true; }
    if( pFS->MseOver() ) { mseOver = true; idx_mse = -1; if( button::didScroll ) sdataVec[idx_curr].xCurr = pFS->fVal; return true; }
    for( size_t i = 0; i < selButtVec.size(); ++i ) if( selButtVec[i].MseOver() ) { mseOver = true; idx_mse = i; return true; }
    idx_mse = -1;
    mseOver = false;
    return false;
}

bool multiSelector::hitLeft()
{
 //   if( button::clickEvent_Lt() != 1 ) return false;
    if( !(sel && button::clickEvent_Lt() == 1) ) return false;

    if( idx_mse > -1 )// over a sel button
    {
        selButtVec[idx_curr].setSel(false);
        idx_curr = idx_mse;
        selButtVec[idx_curr].hitLeft();
        pFS->reInit( sdataVec[idx_curr] );
        return true;
    }
    else if( pFS->mseOver )
    {
        pFS->hitLeft();
        sdataVec[idx_curr].xCurr = pFS->fVal;
        return true;
    }

    return false;
}

bool multiSelector::hitRight()
{
    if( !sel ) return false;

    if( idx_mse > -1 )// over a sel button
    {
        selButtVec[idx_curr].hitRight();
        return true;
    }
    else if( pFS->mseOver && button::clickEvent_Rt() == 1 )
    {
        pFS->iVal = sdataVec[idx_curr].xCurr = sdataVec[idx_curr].xInit;
        pFS->hitRight();
        return true;
    }

    return false;
}

void multiSelector::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - pos;
    pFS->setPosition( pFS->pos + dPos );
    for( auto& butt : selButtVec ) butt.setPosition( butt.pos + dPos );
    pos = Pos;
}

/*
bool multiSelector::MseOver()
{
    if( pFS->MseOver() ) { pButt_mse = nullptr; return true; }
    for( auto& butt : selButtVec ) if( butt.MseOver() ) { pButt_mse = &butt; return true; }
    pButt_mse = nullptr;
    return false;
}

bool multiSelector::hitLeft()
{
    if( button::clickEvent_Lt() == 0 ) return false;// no click event

    if( pButt_mse )// over a sel button
    {
        pBR_curr->setSel(false);
        pBR_curr = pButt_mse;
        pButt_mse->hitLeft();
        return true;
    }
    else if( pFS->mseOver )
    {
        pFS->hitLeft();
        return true;
    }

    return false;
}
*/
