#include "lvl_scrabble.h"

sf::Vector2f Tile::tileSz(40.0f,40.0f);
sf::Color Tile::tileColor(200,200,200);
sf::Vector2f Tile::ltrOfst(10.0f,10.0f), Tile::valueOfst(25.0f,25.0f);
const sf::Font* Tile::pFont = nullptr;
unsigned int Tile::ltrSz = 16, Tile::valueSz = 6;
unsigned int lvl_scrabble::poolIdx = 0;

void Tile::init( char Ltr, unsigned int Value )
{
    ltr = Ltr;
    value = Value;
    rect.setFillColor( Tile::tileColor );
    rect.setOutlineColor( sf::Color::Blue );
    rect.setOutlineThickness( -2.0f );
    rect.setSize( Tile::tileSz );
    if( Tile::pFont )
    {
        ltrTxt.setFont( *Tile::pFont );
        ltrTxt.setColor( sf::Color::Black );
        ltrTxt.setCharacterSize( Tile::ltrSz );
        ltrTxt.setString( sf::String( Ltr ) );
        valueTxt.setFont( *Tile::pFont );
        valueTxt.setColor( sf::Color::Black );
        valueTxt.setCharacterSize( Tile::valueSz );
        if( Value == 10 )
            valueTxt.setString( sf::String( "10" ) );
        else
        {
            char valChar = '0' + (char)Value;
            valueTxt.setString( sf::String( valChar ) );
        }
    }
    else std::cout << "\nTile Font no good";

    return;
}

void Tile::draw( sf::RenderTarget& RT ) const
{
    RT.draw( rect );
    RT.draw( ltrTxt );
    RT.draw( valueTxt );
}

void Tile::setPosition( sf::Vector2f Pos )
{
    rect.setPosition( Pos );
    ltrTxt.setPosition( Pos + ltrOfst );
    valueTxt.setPosition( Pos + valueOfst );
}

bool Tile::hit( sf::Vector2f hitPos ) const
{
    sf::Vector2f pos = rect.getPosition(), sz = rect.getSize();
    if( hitPos.x < pos.x ) return false;
    if( hitPos.x > pos.x + sz.x ) return false;
    if( hitPos.y < pos.y ) return false;
    if( hitPos.y > pos.y + sz.y ) return false;
    return true;
}

//bool wordPlay::verify( const std::vector<Tile*>& rpHand ) const
bool wordPlay::verify( const std::vector<Tile*>& rpHand, std::function<void(Tile*,unsigned int)> pFunc ) const
{
    std::vector<bool> checkList( rpHand.size(), true );// all tiles held
    unsigned int i = 0;
    for( i = 0; i < word.length(); ++i )
    {
        if( i == Midx ) continue;// existing letter
        unsigned int j = 0;
        for( j = 0; j < rpHand.size(); ++j )
        {
            Tile* pt = rpHand[j];
            if( pt && pt->ltr == word[i] && checkList[j] )
            {
                checkList[j] = false;// tile used
                if( pFunc ) pFunc( pt, i );
                break;
            }
        }

        if( j == rpHand.size() )
        {
            // is a blank tile held?
            unsigned int k = 0;
            for( k = 0; k < rpHand.size(); ++k )
            {
                Tile* pt = rpHand[k];
                if( pt->ltr == ' ' && checkList[k] )
                {
                    checkList[k] = false;
                    if( pFunc ) pFunc( pt, i );
                    else std::cout << " with blank = " << word[i];
                    break;
                }
            }
            if( k == rpHand.size() )break;// blank not found either
        }
    }

    return i == word.length();
}

// scrabble mfs
bool lvl_scrabble::init()
{
    std::ifstream fin("include/levels/scrabble/initData_bin.txt");
    if( !fin ) return false;

    // static values
    fin >> Tile::tileSz.x; Tile::tileSz.y = Tile::tileSz.x;
    fin >> Tile::ltrSz >> Tile::valueSz;

    fin >> wndWSizeFactor.x >> wndWSizeFactor.y;

    Level::winW = Tile::tileSz.x*wndWSizeFactor.x;
    Level::winH = Tile::tileSz.y*wndWSizeFactor.y;

    Level::quitButt.setPosition( {Level::winW/2.0f+100.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::quitButt );
    Level::goto_MMButt.setPosition( {Level::winW/2.0f,Level::winH-50.0f} );// not last assignment? Appears near (wndW/2, wndwH)
    button::RegisteredButtVec.push_back( &Level::goto_MMButt );
    Level::clearColor = sf::Color(50,0,0);
    for( size_t i=0; i<4; ++i ) button::moValBox[i].color = Level::clearColor;

    char Ltr = 'A';
    unsigned int Value = 1, qty = 1;
    fin >> Tile::ltrOfst.x >> Tile::ltrOfst.y >> Tile::valueOfst.x >> Tile::valueOfst.y;
    for( Ltr = 'A'; (Ltr <= 'Z')&&(fin >> qty >> Value); ++Ltr )// each letter
    {
        for( unsigned j = 0; j < qty; ++j )
        {
            TileVec.push_back( Tile( Ltr, Value ) );
            TileVec.back().setPosition( sf::Vector2f(-100.0f, 400.0f) );
        }
        ltrValue[Ltr-'A'] = Value;
    }

//    playerHand.resize(7);
 //   pcHand.resize(7);
    pTilesPlayer.resize(7);
    pTilesPC.resize(7);
    playerLegVec.resize(7);
    pcLegVec.resize(7);
    legAdaptVec.resize(7);

    // 2 blanks
    TileVec.push_back( Tile( ' ', 0 ) );
    TileVec.back().setPosition( sf::Vector2f(-100.0f, 400.0f) );
    TileVec.push_back( Tile( ' ', 0 ) );
    TileVec.back().setPosition( sf::Vector2f(-100.0f, 400.0f) );

    std::cerr << "\ninit(): TileVec.size() = " << TileVec.size();
    std::random_shuffle( TileVec.begin(), TileVec.end() );

    playersTileArea.setFillColor( (playersTurn ? sf::Color::Cyan : sf::Color(100,100,100)) );
    playersTileArea.setOutlineColor( sf::Color::Green );
    playersTileArea.setOutlineThickness( -2.0f );
    playersTileArea.setSize( sf::Vector2f( 8.0f*Tile::tileSz.x, 2.0f*Tile::tileSz.y ) );
    playersTileArea.setPosition( 3.0f*Tile::tileSz.x, 18.0f*Tile::tileSz.y );

    pcTileArea.setFillColor( (playersTurn ? sf::Color(100,100,100) : sf::Color::Cyan) );
    pcTileArea.setOutlineColor( sf::Color::Green );
    pcTileArea.setOutlineThickness( -2.0f );
    pcTileArea.setSize( sf::Vector2f( 2.0f*Tile::tileSz.y, 8.0f*Tile::tileSz.x ) );
    pcTileArea.setPosition( 18.0f*Tile::tileSz.x, 8.0f*Tile::tileSz.y );

    passArea.setFillColor( sf::Color::White );
    passArea.setOutlineColor( sf::Color::Red );
    passArea.setOutlineThickness( -2.0f );
    passArea.setSize( sf::Vector2f( 8.0f*Tile::tileSz.y, 2.0f*Tile::tileSz.x ) );
    passArea.setPosition( 3.0f*Tile::tileSz.x, 14.0f*Tile::tileSz.y );

    selRect.setFillColor( sf::Color( 0,0,100,100 ) );
  //  selRect.setOutlineColor( sf::Color::Blue );
  //  selRect.setOutlineThickness( -2.0f );
    selRect.setSize( Tile::tileSz );
    selRect.setPosition( 0.0f, 0.0f );
    showPlayRect.setFillColor( sf::Color( 100,0,0,100 ) );


    // the board
    if( !createBoard() ) return false;
    if( !boardTxt.loadFromImage( boardImg ) ) return false;
    boardSpr.setTexture( boardTxt );
//    boardSpr.setImage( boardImg );
    boardSpr.setScale( 1.0f, 1.0f );
    boardPos.x = Tile::tileSz.x; boardPos.y = Tile::tileSz.y;
    boardSpr.setPosition( boardPos );

    sf::Text label("Deal", *button::pFont, 12);
    label.setColor( sf::Color::Black );
    std::function<void(void)> pHitFunc = [this]()
    {
        if( !anyMoving )
        {
            if( playersTurn ) startTileDeal( pTilesPlayer, playersTileArea, playerLegVec);
            else startTileDeal( pTilesPC, pcTileArea, pcLegVec);
        }
    };
    startButt.init( 18.0f*Tile::tileSz.x, 18.0f*Tile::tileSz.y, 50.0f, 25.0f, pHitFunc, label );
    startButt.mode = 'I';
    button::RegisteredButtVec.push_back( &startButt );

    label.setString("turn");
    std::function<void(void)> pHF = [this](){ playersTurn = turnButt.sel;
        if( playersTurn ) { playersTileArea.setFillColor( sf::Color::Cyan ); pcTileArea.setFillColor( sf::Color(100,100,100) ); }
        else { playersTileArea.setFillColor( sf::Color(100,100,100) ); pcTileArea.setFillColor( sf::Color::Cyan ); }
    };
    turnButt.init( 18.0f*Tile::tileSz.x + 80.0f, 18.0f*Tile::tileSz.y, 50.0f, 25.0f, pHF, label );
    turnButt.setSel( playersTurn );
    button::RegisteredButtVec.push_back( &turnButt );

    label.setString("pass");
    playPassButt.init( 16.0f*Tile::tileSz.x, 18.0f*Tile::tileSz.y, 50.0f, 25.0f, nullptr, label );
    button::RegisteredButtVec.push_back( &playPassButt );

    // ani control
    label.setString("delay");
    delayStrip.init( 18.5f*Tile::tileSz.x, 1.5f*Tile::tileSz.y, 120.0f, 20.0f, label, 0.0f, 40.0f, dealDelay, &dealDelay );
    label.setString("accel");
    accStrip.init( 18.5f*Tile::tileSz.x, 3.0f*Tile::tileSz.y, 120.0f, 20.0f, label, -1.0f, 0.0f, -0.5, nullptr );
    label.setString("speed final");
    velStrip.init( 18.5f*Tile::tileSz.x, 4.5f*Tile::tileSz.y, 120.0f, 20.0f, label, 0.0f, 20.0f, 1.0, nullptr );
    tileAniControl.init( sf::Vector2f(20.0f*Tile::tileSz.x,5.0f), sf::Vector2f(18.0f*Tile::tileSz.x,Tile::tileSz.y), sf::Vector2f(5.0f*Tile::tileSz.x,5.0f*Tile::tileSz.y), "Animation" );
    tileAniControl.pButtVec.push_back( &delayStrip );
    tileAniControl.pButtVec.push_back( &accStrip );
    tileAniControl.pButtVec.push_back( &velStrip );
    button::RegisteredButtVec.push_back( &tileAniControl );

    // making word plays
    label.setString("Find Best");
    std::function<void(void)> p_HitFunc = [this]()
    {
        highPlay.score = 0;
        for( unsigned int r=0; r<15; ++r)
        for( unsigned int c=0; c<15; ++c)
            showPlayBounds( r, c, false );

        if( highPlay.score > 0 )
        {
            std::cout << "\n** high: " << highPlay.word << " for " << highPlay.score;
            highPlay.verify( playersTurn ? pTilesPlayer : pTilesPC );
            std::cout << "\ndir: " << highPlay.dir << " to: " << highPlay.row << ", " << highPlay.col;

        }
    };
    bestPlayButt.init( 21.0f*Tile::tileSz.x, 10.0f*Tile::tileSz.y, 70.0f, 35.0f, p_HitFunc, label );
    bestPlayButt.mode = 'I';
    button::RegisteredButtVec.push_back( &bestPlayButt );

    label.setString("Set Word");
    findWordButt.init( 21.0f*Tile::tileSz.x, 12.0f*Tile::tileSz.y, 100.0f, 50.0f, [this](){ if( playPassButt.sel ) tradeTiles(); else setWord(); showPlay = false; }, label );
    findWordButt.mode = 'I';
    button::RegisteredButtVec.push_back( &findWordButt );

    label.setString("Save Game");
    saveButt.init( 21.0f*Tile::tileSz.x, Level::winH-50.0f, 60.0f, 30.0f, [this](){ saveGame(); }, label );
    button::RegisteredButtVec.push_back( &saveButt );
    label.setString("Load Game");
    loadSavedButt.init( 21.0f*Tile::tileSz.x + 80.0f, Level::winH-50.0f, 60.0f, 30.0f, [this](){ loadSavedGame(); }, label );
    button::RegisteredButtVec.push_back( &loadSavedButt );

    label.setString("blank letter");
    p_HitFunc = [this](){ if( pBlankTile ) pBlankTile->init( 'A'+blankButt.selIdx, 0 ); blankButt.setSel(false); };
    blankButt.init( 6.0f*Tile::tileSz.x, 14.0f*Tile::tileSz.y, 180.0f, 150.0f, 5, 6, p_HitFunc, label );
    button::RegisteredButtVec.push_back( &blankButt );
    lettersTxt.loadFromFile( "images/letters.png" );
    blankButt.R.setTexture( &lettersTxt );

    wordPlayedMsg.setFont( *button::pFont );
    wordPlayedMsg.setCharacterSize( 20 );
    wordPlayedMsg.setColor( sf::Color::Red );
    wordPlayedMsg.setPosition( 21.0f*Tile::tileSz.x, 14.0f*Tile::tileSz.y );

    wordScoreMsg.setFont( *button::pFont );
    wordScoreMsg.setCharacterSize( 18 );
    wordScoreMsg.setColor( sf::Color::White );
    wordScoreMsg.setPosition( 21.0f*Tile::tileSz.x, 15.0f*Tile::tileSz.y );

    playScrMsg.setFont( *button::pFont );
    playScrMsg.setString( "Player: 0" );
    playScrMsg.setCharacterSize( 18 );
    playScrMsg.setColor( sf::Color::White );
    playScrMsg.setPosition( 21.0f*Tile::tileSz.x, 16.0f*Tile::tileSz.y );
    pcScrMsg.setFont( *button::pFont );
    pcScrMsg.setString( "PC:     0" );
    pcScrMsg.setCharacterSize( 18 );
    pcScrMsg.setColor( sf::Color::White );
    pcScrMsg.setPosition( 21.0f*Tile::tileSz.x, 17.0f*Tile::tileSz.y );

 //   startTileDeal();

    return true;
}

void lvl_scrabble::update( float dt )
{
    if( !anyMoving )
    {
        if( button::clickEvent_Lt() == 1 )// check for hand hits
            startTile_toBoard();// selection and placemnt
        else if( button::clickEvent_Rt() == 1 )// return tile to hand
            startTile_toHand();// return tile to hand
        return;
    }

    anyMoving = false;
    for( unsigned int i = 0; i < legAdaptVec.size(); ++i )
    {
        legAdaptVec[i].update( dt );
        if( legAdaptVec[i].pLeg ) anyMoving = true;
    }

    if( !anyMoving )
    {
        if( pSelTile && pSelTile->ltr == ' ' )// assign letter
        {
            pBlankTile = pSelTile;
            blankButt.setSel(true);
        }
        pSelTile = nullptr;
        std::cout << "\nMotion finished";
    }
}

void lvl_scrabble::draw( sf::RenderTarget& RT ) const
{
    RT.draw( boardSpr );
    RT.draw( playersTileArea );
    RT.draw( pcTileArea );
    for( unsigned int i = 0; i < 15*15; ++i ) if( pTileBoard[i/15][i%15] ) pTileBoard[i/15][i%15]->draw(RT);
    if( playPassButt.sel ) RT.draw( passArea );
    for( const Tile* ptl : pTilesPlayer ) if( ptl) ptl->draw(RT);
    for( const Tile* ptl : pTilesPC ) if( ptl) ptl->draw(RT);
    if( pSelTile ) RT.draw( selRect );
    if( showPlay ) RT.draw( showPlayRect );

    RT.draw( wordPlayedMsg );
    RT.draw( wordScoreMsg );
    RT.draw( playScrMsg );
    RT.draw( pcScrMsg );
}

bool lvl_scrabble::handleEvent( sf::Event& rEvent )
{
    if ( rEvent.type == sf::Event::KeyPressed )
    {
        if ( rEvent.key.code == sf::Keyboard::P )// make highPlay
        {
            if( highPlay.score == 0 || highPlay.word.length() < 2 ) return true;
            makeHighPlay();
        }
    }

    return true;
}

bool lvl_scrabble::createBoard()
{
    unsigned int dim = abs( floor( Tile::tileSz.x ) );
    boardImg.create( 16*dim, 16*dim, sf::Color::Blue );
    std::ifstream fin("include/levels/scrabble/boardSquareTypes.txt");
    if( !fin ) return false;

    sf::Texture DLtxt, DWtxt, TLtxt, TWtxt;
    if( !DLtxt.loadFromFile("images/DoubleLetter.png") ) return false;
    if( !DWtxt.loadFromFile("images/DoubleWord.png") ) return false;
    if( !TLtxt.loadFromFile("images/TrippleLetter.png") ) return false;
    if( !TWtxt.loadFromFile("images/TrippleWord.png") ) return false;
    const sf::Texture* pSqrTxt = nullptr;

    sf::Color dwClr( 255, 128, 192 ), twClr( 255, 0, 128 );
    sf::Color slClr( 255, 128, 0 ), dlClr( 0, 255, 255 ), tlClr( 0, 128, 255 );

    for( unsigned int r = 0; r < 15; ++r )
        for( unsigned int c = 0; c < 15; ++c )
        {
            pTileBoard[r][c] = nullptr;
            pSqrTxt = nullptr;
            char sqrType;
            sf::Color sqrClr;
            if( !( fin >> sqrType ) ) return false;
            spaceType[r][c] = sqrType;
            switch( sqrType )
            {
                case 's': sqrClr = slClr; break;// single letter
                case 'd': sqrClr = dlClr; pSqrTxt = &DLtxt; break;// single letter
                case 't': sqrClr = tlClr; pSqrTxt = &TLtxt; break;// single letter
                case 'D': sqrClr = dwClr; pSqrTxt = &DWtxt; break;// double word
                case 'T': sqrClr = twClr; pSqrTxt = &TWtxt; break;// triple word
                default: sqrClr = sf::Color::Black; break;
            }

            for( unsigned int ri = 0; ri < dim; ++ri )
                for( unsigned int ci = 0; ci < dim; ++ci )
                    boardImg.setPixel ( dim/2 + dim*c + ci, dim/2 + dim*r + ri, sqrClr );

            if( sqrType != 's' )// copy text to image
            {
            //    unsigned int fontSz = 30;
            //    sf::Glyph glyph = Tile::pFont->getGlyph(sqrType, fontSz, false);
            //    sf::Texture bitmap = Tile::pFont->getTexture(fontSz);
             //   sf::Image image;
             //   image.create(glyph.bounds.width, glyph.bounds.height);
             //   boardImg.copy(bitmap.copyToImage(), dim/2 + dim*c + 10, dim/2 + dim*r + 10, glyph.textureRect);

                if( pSqrTxt )// score txt to copy
                {
                    boardImg.copy( pSqrTxt->copyToImage(), dim/2 + dim*c + 10, dim/2 + dim*r + 10 );
                }
            }
        }

    for( unsigned int i = 0; i < 15; ++i )// which line
        for( unsigned int j = 0; j < 15*dim; ++j )// along line
        {
            boardImg.setPixel ( dim/2 + j, dim/2 + dim*i, sf::Color::Black );// horiz
            boardImg.setPixel ( dim/2 + dim*i, dim/2 + j, sf::Color::Black );// vertical
        }

    return true;
}

void lvl_scrabble::startTileDeal( std::vector<Tile*>& pHand, const sf::RectangleShape handArea, std::vector<linLeg>& legVec )
{
    anyMoving = true;
    pSelTile = nullptr;
    std::cout << "\nMotion started";
    sf::Vector2f handPos = handArea.getPosition();
    sf::Vector2f iPos( 600.0f, -100.0f ), dPos( 0.0f, 0.0f );
    sf::Vector2f handFpos( handPos.x + 0.5f*Tile::tileSz.x, handPos.y + Tile::tileSz.y*0.5f );//, pcFpos( pcAreaPos.x + 0.5f*Tile::tileSz.x, pcAreaPos.y + Tile::tileSz.y*0.5f );
    if( handArea.getSize().x > handArea.getSize().y ) dPos.x = Tile::tileSz.x;// horiz
    else dPos.y = Tile::tileSz.y;// horiz

    legVec.resize(7);
    legAdaptVec.resize(7);

    for( unsigned int i = 0; (i < legVec.size())&&(i < pHand.size()); ++i )
    {
        pHand[i] = &TileVec[poolIdx];
        Tile* pTile = pHand[i];
        legAdaptVec[i].init( legVec[i], [pTile]( float x, float y ) { if( pTile ) pTile->setPosition(x,y); }, iPos.x, iPos.y, handFpos.x + i*dPos.x, handFpos.y + i*dPos.y, velStrip.fVal, accStrip.fVal, i*delayStrip.fVal );
        ++poolIdx; poolIdx %= TileVec.size();
    }

    std::cout << "\npoolIdx = " << poolIdx;
}

void lvl_scrabble::startTile_toBoard()// selection and placemnt
{
    std::vector<Tile*>* pHand = nullptr;
    if( hitRect( playersTileArea.getGlobalBounds(), button::msePos() ) ) pHand = &pTilesPlayer;// player
    else if( hitRect( pcTileArea.getGlobalBounds(), button::msePos() ) ) pHand = &pTilesPC;// pc

    if( pHand )
    {
        for( Tile* rt: *pHand )
            if( rt && rt->hit( button::msePos() ) )
            {
                pSelTile = rt;
                selRect.setPosition( rt->rect.getPosition() );
                selRect.setSize( rt->rect.getSize() );
                showPlay = false;
                break;
            }
    }
    else// to board or passArea
    {
        sf::Vector2f hitOfst = button::msePos() - boardPos - Tile::tileSz/2.0f;

        if( playPassButt.sel )// selected tile to passArea
        {
            sf::FloatRect paGB = passArea.getGlobalBounds();
            if( pSelTile && hitRect( paGB, button::msePos() ) )// passArea hit
            {
                sf::Vector2f iPos = pSelTile->rect.getPosition();
                sf::Vector2f fPos = button::msePos() - Tile::tileSz/2.0f;
                sf::FloatRect fRect( fPos.x, fPos.y, Tile::tileSz.x, Tile::tileSz.y );
                fitAinB( fRect, paGB );
                legAdaptVec[0].init( playerLegVec[0], [this]( float x, float y ) { pSelTile->setPosition(x,y); }, iPos.x, iPos.y, fRect.left, fRect.top, velStrip.fVal, accStrip.fVal );
                anyMoving = true;
            }
         //   else pSelTile = nullptr;// cancel selection
        }
        else if( (hitOfst.x > 0.0f)&&(hitOfst.x < 15.0f*Tile::tileSz.x)&&(hitOfst.y > 0.0f)&&(hitOfst.y < 15.0f*Tile::tileSz.y) )
        {
            unsigned int col = floor(hitOfst.x/Tile::tileSz.x);
            unsigned int row = floor(hitOfst.y/Tile::tileSz.y);

            if( pSelTile )// to board
            {
                sf::Vector2f fPos = boardPos + 0.5f*Tile::tileSz;
                fPos.x += col*Tile::tileSz.x;
                fPos.y += row*Tile::tileSz.y;
                legAdaptVec[0].init( playerLegVec[0], [this]( float x, float y ) { pSelTile->setPosition(x,y); }, pSelTile->rect.getPosition().x, pSelTile->rect.getPosition().y, fPos.x, fPos.y, velStrip.fVal, accStrip.fVal );
                anyMoving = true;
            }
            else// show play bounds
            {
                highPlay.score = 0;
                showPlayBounds( row, col );
            }
        }
        else pSelTile = nullptr;// cancel selection
    }
}

void lvl_scrabble::startTile_toHand()// return tile to hand
{
    std::vector<Tile*>& rHand = playersTurn ? pTilesPlayer : pTilesPC;
    for( unsigned int i = 0; i < rHand.size(); ++i )
    {
        if( rHand[i] && rHand[i]->hit( button::msePos() ) )
        {
            sf::Vector2f iPos = rHand[i]->getPosition();
            sf::Vector2f pos = playersTurn ? playersTileArea.getPosition() : pcTileArea.getPosition();
            sf::Vector2f dPos = playersTurn ? sf::Vector2f(Tile::tileSz.x, 0.0f) : sf::Vector2f(0.0f, Tile::tileSz.y);
            pos.x += Tile::tileSz.x*0.5f + dPos.x*i;
            pos.y += Tile::tileSz.y*0.5f + dPos.y*i;

            Tile* pTile = rHand[i];
            if( rHand[i]->value == 0 ) rHand[i]->init( ' ', 0 );
            legAdaptVec[0].init( playerLegVec[0], [pTile]( float x, float y ) { pTile->setPosition(x,y); }, iPos.x, iPos.y, pos.x, pos.y, velStrip.fVal, accStrip.fVal );
            anyMoving = true;
        }
    }
}

bool lvl_scrabble::hitBoard( sf::Vector2f hitPt, unsigned int& row, unsigned int& col ) const
{
    sf::Vector2f hitOfst = hitPt - boardPos - Tile::tileSz/2.0f;
    if( (hitOfst.x >= 0.0f)&&(hitOfst.x < 15.0f*Tile::tileSz.x)&&(hitOfst.y >= 0.0f)&&(hitOfst.y < 15.0f*Tile::tileSz.y) )
    {
        col = floor(hitOfst.x/Tile::tileSz.x);
        row = floor(hitOfst.y/Tile::tileSz.y);
        return true;
    }
    return false;
}

void lvl_scrabble::setWord()
{
    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;
    std::string word;
    unsigned int row, minRow = 15, maxRow = 0;
    unsigned int col, minCol = 15, maxCol = 0;
    unsigned int tilesPlaced = 0, tileIdx = 0;

    unsigned int wordScore = 0, scoreFactor = 1;
 //   std::cout << "\nhand: ";
    for( Tile*& pt : rpHand )
    {
        if( !pt ) continue;

        if( hitBoard( pt->getPosition(), row, col ) )
        {
            if( col < minCol ) minCol= col;
            if( col > maxCol ) maxCol = col;

            if( row < minRow ) minRow = row;
            if( row > maxRow ) maxRow = row;

            pTileBoard[row][col] = pt;

       /*     unsigned int dim = abs( floor( Tile::tileSz.x ) );
            if( true )// copy text to image
            {
                for( unsigned int c=0; c<dim; ++c )
                for( unsigned int r=0; r<dim; ++r )
                    if( r > 1 && r+2 < dim && c > 1 && c+2 < dim ) boardImg.setPixel ( dim/2 + dim*col + c, dim/2 + dim*row + r, Tile::tileColor );
                    else boardImg.setPixel ( dim/2 + dim*col + c, dim/2 + dim*row + r, sf::Color::Blue );

                sf::Glyph glyLtr = Tile::pFont->getGlyph(pt->ltr, Tile::ltrSz, false);
                sf::Texture bmpLtr = Tile::pFont->getTexture(Tile::ltrSz);
                boardImg.copy(bmpLtr.copyToImage(), dim/2 + dim*col + Tile::ltrOfst.x, dim/2 + dim*row +  Tile::ltrOfst.y, glyLtr.textureRect);
                sf::Glyph glyScr = Tile::pFont->getGlyph(pt->value+'0', Tile::valueSz, false);
                sf::Texture bmpScr = Tile::pFont->getTexture(Tile::valueSz);
                boardImg.copy(bmpScr.copyToImage(), dim/2 + dim*col + Tile::valueOfst.x, dim/2 + dim*row +  Tile::valueOfst.y, glyScr.textureRect);
                boardTxt.update( boardImg );
            }   */

            if( spaceType[row][col] == 'D' ) scoreFactor *= 2;// double word
            else if( spaceType[row][col] == 'T' ) scoreFactor *= 3;// triple word
            else if( spaceType[row][col] == 'd' ) wordScore += pt->value;// double letter
            else if( spaceType[row][col] == 't' ) wordScore += 2*pt->value;// triple letter

            sf::Vector2f iPos( 600.0f, -100.0f );
            sf::Vector2f pos = playersTurn ? playersTileArea.getPosition() : pcTileArea.getPosition();
            sf::Vector2f dPos = playersTurn ? sf::Vector2f(Tile::tileSz.x, 0.0f) : sf::Vector2f(0.0f, Tile::tileSz.y);
            pt = &TileVec[poolIdx];
            pos.x += Tile::tileSz.x*0.5f + dPos.x*tileIdx;
            pos.y += Tile::tileSz.y*0.5f + dPos.y*tileIdx;
            legAdaptVec[tilesPlaced].init( playerLegVec[tilesPlaced], [pt]( float x, float y ) { pt->setPosition(x,y); }, iPos.x, iPos.y, pos.x, pos.y, velStrip.fVal, accStrip.fVal, tilesPlaced*delayStrip.fVal );
            anyMoving = true;
            pt->setPosition( iPos );
            ++poolIdx; poolIdx %= TileVec.size();
            ++tilesPlaced;
        }
        else ++pt->turnsHeld;// keeper
        ++tileIdx;

   //     std::cout << pt->ltr << pt->turnsHeld << ' ';
    }
//    std::cout << '\n';
    handReport( rpHand );

    if( tilesPlaced > 0 )// word made
    {
        if( minRow == maxRow )// horiz
        {
            std::cout << "\nhorizontal play";
         //   if( minCol > 0 && pTileBoard[minRow][minCol-1] ) word.push_back( pTileBoard[minRow][minCol-1]->ltr );// leading tile
            while( minCol > 0 && pTileBoard[minRow][minCol-1] ) --minCol;// word.push_back( pTileBoard[minRow][minCol-1]->ltr );// leading tile
            while( maxCol < 14 && pTileBoard[minRow][maxCol+1] ) ++maxCol;
            for( unsigned int i = minCol; i <= maxCol; ++i )
                if( pTileBoard[minRow][i] )
                {
                    word.push_back( pTileBoard[minRow][i]->ltr );
                    wordScore += pTileBoard[minRow][i]->value;
                }
        }
        else if( minCol == maxCol )// vertical
        {
            std::cout << "\nvertical play";
        //    if( minRow > 0 && pTileBoard[minRow-1][minCol] ) word.push_back( pTileBoard[minRow-1][minCol]->ltr );// leading tile
            while( minRow > 0 && pTileBoard[minRow-1][minCol] ) --minRow;// word.push_back( pTileBoard[minRow-1][minCol]->ltr );// leading tile
            while( maxRow < 14 && pTileBoard[maxRow+1][minCol] ) ++maxRow;
            for( unsigned int i = minRow; i <= maxRow; ++i )
                if( pTileBoard[i][minCol] )
                {
                    word.push_back( pTileBoard[i][minCol]->ltr );
                    wordScore += pTileBoard[i][minCol]->value;
                }
        }

        wordPlayedMsg.setString( word.c_str() );
        if( wordInList( word ) ) wordPlayedMsg.setColor( sf::Color::Green );
        else wordPlayedMsg.setColor( sf::Color::Red );

        wordScore *= scoreFactor;
        to_SF_string( wordScoreMsg, wordScore );
        sf::Text scrMsg;

        if( playersTurn )
        {
            playerScore += wordScore;
            to_SF_string( scrMsg, playerScore );
            playScrMsg.setString( "Player: " + scrMsg.getString() );
        }
        else
        {
            pcScore += wordScore;
            to_SF_string( scrMsg, pcScore );
            pcScrMsg.setString( "PC:     " + scrMsg.getString() );
        }

        std::cout << "\npoolIdx = " << poolIdx;
    }
}

void lvl_scrabble::tradeTiles()
{
    if( !playPassButt.sel ) return;

    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;
    unsigned int numTrading = 0;
    for( Tile*& pt : rpHand )
        if( pt && passArea.getGlobalBounds().intersects( pt->rect.getGlobalBounds() ) ) ++numTrading;

    // index to replacement tiles
    unsigned int stride = ( TileVec.size() - poolIdx )/( 1 + numTrading );// evenly divide numberr remaining
    std::cout << "\npoolIdx = " << poolIdx << " newIdx:";
//    for( Tile*& pt : rpHand )
    for( unsigned int i = 0; i < rpHand.size(); ++i )
        if( rpHand[i] && numTrading > 0 && passArea.getGlobalBounds().intersects( rpHand[i]->rect.getGlobalBounds() ) )
        {
            Tile tmpTile = *rpHand[i];// traded tile
            tmpTile.turnsHeld = 0;
            unsigned int newIdx = poolIdx + stride*numTrading;
            *rpHand[i] = TileVec[ newIdx ];// new tile to hand
            TileVec[ newIdx ] = tmpTile;// traded tile to pool
            std::cout << " " << newIdx;
            --numTrading;
            sf::Vector2f pos = playersTurn ? playersTileArea.getPosition() : pcTileArea.getPosition();
            sf::Vector2f dPos = playersTurn ? sf::Vector2f(Tile::tileSz.x, 0.0f) : sf::Vector2f(0.0f, Tile::tileSz.y);
            pos.x += Tile::tileSz.x*0.5f + dPos.x*i;
            pos.y += Tile::tileSz.y*0.5f + dPos.y*i;
            Tile* pt = rpHand[i];
            legAdaptVec[numTrading].init( playerLegVec[numTrading], [pt,tmpTile]( float x, float y ) { pt->setPosition(x,y); }, tmpTile.getPosition().x, tmpTile.getPosition().y, pos.x, pos.y, velStrip.fVal, accStrip.fVal );//, numTrading*delayStrip.fVal );
            anyMoving = true;
        }

    handReport( rpHand );
}

void lvl_scrabble::showPlayBounds( unsigned int row, unsigned int col, bool echoAll )
{
    if( !pTileBoard[row][col] ) { showPlay = false; return; }// no tile at given location

    showPlay = true;
    unsigned int minRow = row, maxRow = row;
    unsigned int minCol = col, maxCol = col;


    if( col > 0 )// look left
    {
        //       left                                   left-left                                  right
        if( !pTileBoard[row][col-1]&&( col == 1 || !pTileBoard[row][col-2] )&&( col == 14 || !pTileBoard[row][col+1] )
        //              above                               below
        &&( row == 0 || !pTileBoard[row-1][col-1] )&&( row == 14 || !pTileBoard[row+1][col-1] ) )
        {
            --minCol;//                        left                                      above                                below
            while( minCol > 0 && ( minCol == 1 || !pTileBoard[row][minCol-2] )&&( row == 0 || !pTileBoard[row-1][minCol-1] )&&( row == 14 || !pTileBoard[row+1][minCol-1] ) )--minCol;
        }
    }
    if( col < 14 )// look right
    {
        //       right                                   right-right                                 left
        if( !pTileBoard[row][col+1]&&( col == 13 || !pTileBoard[row][col+2] )&&( col == 0 || !pTileBoard[row][col-1] )
        //              above                               below
        &&( row == 0 || !pTileBoard[row-1][col+1] )&&( row == 14 || !pTileBoard[row+1][col+1] ) )
        {
            ++maxCol;//                         right                                      above                                             below
            while( maxCol < 14 && ( maxCol == 13 || !pTileBoard[row][maxCol+2] )&&( row == 0 || !pTileBoard[row-1][maxCol+1] )&&( row == 14 || !pTileBoard[row+1][maxCol+1] ) )++maxCol;
        }
    }

    if( maxCol == minCol )// no horiz play. vert paly?
    {
        if( row > 0 )// look up
        {
            //       above                                   above-above                               below
            if( !pTileBoard[row-1][col]&&( row == 1 || !pTileBoard[row-2][col] )&&( row == 14 || !pTileBoard[row+1][col] )
            //              left                             right
            &&( col == 0 || !pTileBoard[row-1][col-1] )&&( col == 14 || !pTileBoard[row-1][col+1] ) )
            {
                --minRow;//                        above                                      left                                right
                while( minRow > 0 && ( minRow == 1 || !pTileBoard[minRow-2][minCol] )&&( col == 0 || !pTileBoard[minRow-1][col-1] )&&( col == 14 || !pTileBoard[minRow-1][col+1] ) )--minRow;
            }
        }
        if( row < 14 )// look down
        {
            //       below                                   below-below                              above
            if( !pTileBoard[row+1][col]&&( row == 13 || !pTileBoard[row+2][col] )&&( row == 0 || !pTileBoard[row-1][col] )
            //              left                             right
            &&( col == 0 || !pTileBoard[row+1][col-1] )&&( col == 14 || !pTileBoard[row+1][col+1] ) )
            {
                ++maxRow;//                        above                                      left                                right
                while( maxRow < 14 && ( maxRow == 13 || !pTileBoard[maxRow+2][col] )&&( col == 0 || !pTileBoard[maxRow+1][col-1] )&&( col == 14 || !pTileBoard[maxRow+1][col+1] ) )++maxRow;
            }
        }
    }

    sf::Vector2f rectPos = pTileBoard[row][col]->rect.getPosition();
    sf::Vector2f rectSz = Tile::tileSz;
    rectSz.x += ( maxCol - minCol )*Tile::tileSz.x;
    rectPos.x -= ( col - minCol )*Tile::tileSz.x;
    rectSz.y += ( maxRow - minRow )*Tile::tileSz.y;
    rectPos.y -= ( row - minRow )*Tile::tileSz.y;
    showPlayRect.setSize( rectSz );
    showPlayRect.setPosition( rectPos );

 //   highPlay.score = 0;
    if( maxCol == minCol ) findWordTemplates( minRow, maxRow, row, col, 'V', echoAll );// vertical play
    else findWordTemplates( minCol, maxCol, row, col, 'H', echoAll );// horiz play
}

void lvl_scrabble::findWordTemplates( unsigned int minX, unsigned int maxX, unsigned int row, unsigned int col, char dir, bool echoAll )
{
    if( !pTileBoard[row][col] ) return;// no tile to cross
    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;
    unsigned int X = dir == 'H' ? col : row;
    X -= minX;// now minX is 0
    maxX -= minX;// word space length
    std::string word;
    char Mltr = pTileBoard[row][col]->ltr;
    wordPlay bestPlay;
 //   highPlay.score = 0;
    unsigned int wdLenMax = maxX+1 < 8 ? maxX+1 : 8;
     if( echoAll ) std::cout << "\n** wdLenMax = " << wdLenMax << '\n';

    for( unsigned int wdLen = 2; wdLen <= wdLenMax; ++wdLen )
    {
        word.resize( wdLen );
        unsigned int Midx0 = ( wdLen < X + 1 ) ? wdLen-1 : X;
        unsigned int MidxF = ( maxX - X < wdLen - 1 ) ? wdLen-1 - (maxX-X) : 0;

        if( echoAll ) std::cout << "wdLen = " << wdLen << '\n';
        for( unsigned int i = MidxF; i <= Midx0; ++i )// each M position
        {
            for( unsigned int x = 0; x <= wdLen; ++x )
                if( x == i ) word[x] = Mltr;
                else word[x] = 'x';

            wordPlay currPlay = find_plays( i, wdLen, row, col, dir, echoAll );
            if( currPlay.score > bestPlay.score ) bestPlay = currPlay;
        }
    }

    if( bestPlay.score > 0 )
    {
        std::cout << "\nbest: " << bestPlay.word << " for " << bestPlay.score;
        bestPlay.verify( playersTurn ? pTilesPlayer : pTilesPC );
    }
    if( bestPlay.score > highPlay.score ) highPlay = bestPlay;// build to highplay
}

wordPlay lvl_scrabble::find_plays( unsigned int Midx, unsigned int wdLen, unsigned int row, unsigned int col, char dir, bool echoAll )
{
    wordPlay bestPlay;
    bool anyFound = false;// output control
    if( !pTileBoard[row][col] ) return bestPlay;// no tile to cross
    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;

    if( Midx >= wdLen ) return bestPlay;
    std::ifstream fin;//( fileName.c_str() );

    if( wdLen > 1 + rpHand.size() ) return bestPlay;// not enough tiles in hand
    if( !openDictFile( fin, wdLen ) ) return bestPlay;

    char Mltr = pTileBoard[row][col]->ltr;
    std::string fileWord;
    while( fin >> fileWord )
    {
        if( fileWord[Midx] == Mltr )// candidate
        {
            unsigned int scoreFactor = 1;
            wordPlay thisPlay; thisPlay.init( row, col, Midx, pTileBoard[row][col]->value, fileWord, dir );
            std::function<void(Tile*,unsigned int)> pFunc = [this,&thisPlay,&scoreFactor](Tile* pt,unsigned int i)
            {
                unsigned int r = thisPlay.dir == 'H' ? thisPlay.row : thisPlay.row - thisPlay.Midx + i;
                unsigned int c = thisPlay.dir == 'V' ? thisPlay.col : thisPlay.col - thisPlay.Midx + i;
                thisPlay.score += pt->value;
                if( spaceType[r][c] == 'D' ) scoreFactor *= 2;
                else if( spaceType[r][c] == 'T' ) scoreFactor *= 3;
                else if( spaceType[r][c] == 'd' ) thisPlay.score += pt->value;
                else if( spaceType[r][c] == 't' ) thisPlay.score += 2*pt->value;
            };

            if( thisPlay.verify( rpHand, pFunc ) )
            {
                thisPlay.score *= scoreFactor;
                if( thisPlay.score > bestPlay.score )// new best
                    bestPlay = thisPlay;
               //     bestPlay.init( row, col, Midx, thisPlay.score, fileWord, dir );

                if( !anyFound )
                {
                    if( echoAll )
                    {
                        for( unsigned int j = 0; j < wdLen; ++j ) std::cout << ( j==Midx ? Mltr : 'x' );
                        std::cout << ": ";
                    }
                    anyFound = true;
                }
                if( echoAll ) std::cout << "  " << fileWord << " score: " << bestPlay.score;
            }

       /*     unsigned int wordScore = pTileBoard[row][col]->value, scoreFactor = 1;
            std::vector<bool> checkList( rpHand.size(), true );// all tiles held
            unsigned int i = 0;// which letter in word
            for( ; i < wdLen; ++i )
            {
           //     if( i == Midx ) { wordScore += pTileBoard[row][col]->value; continue; }
                if( i == Midx ) continue;
                unsigned int r = dir == 'H' ? row : row - Midx + i;
                unsigned int c = dir == 'V' ? col : col - Midx + i;
                unsigned int j = 0;// which tile in hand
                for( j = 0; j < rpHand.size(); ++j ) if( rpHand[j]->ltr == fileWord[i] && checkList[j] )
                {
                    checkList[j] = false;
                    wordScore += rpHand[j]->value;
                    if( spaceType[r][c] == 'D' ) scoreFactor *= 2;
                    else if( spaceType[r][c] == 'T' ) scoreFactor *= 3;
                    else if( spaceType[r][c] == 'd' ) wordScore += rpHand[j]->value;
                    else if( spaceType[r][c] == 't' ) wordScore += 2*rpHand[j]->value;
                    break;
                }

                if( j == rpHand.size() )// is a blank tile held?
                {
                    unsigned int k = 0;
                    for( k = 0; k < rpHand.size(); ++k )
                        if( rpHand[k]->ltr == ' ' && checkList[k] )
                        {
                            checkList[k] = false;
                            break;
                        }
                    if( k == rpHand.size() )break;// blank not found either
                }
            }

            if( i == wdLen )// can play this word
            {
                wordScore *= scoreFactor;
                if( wordScore > bestPlay.score )// new best
                    bestPlay.init( row, col, Midx, wordScore, fileWord, dir );

                if( !anyFound )
                {
                    if( echoAll )
                    {
                        for( unsigned int j = 0; j < wdLen; ++j ) std::cout << ( j==Midx ? Mltr : 'x' );
                        std::cout << ": ";
                    }
                    anyFound = true;
                }
                if( echoAll ) std::cout << "  " << fileWord << " score: " << wordScore;
            }   */

        }// end if fileWord is candidate
    }

    if( echoAll && anyFound ) std::cout << '\n';

     return bestPlay;
}

/*
wordPlay lvl_scrabble::find_plays( unsigned int Midx, unsigned int wdLen, unsigned int row, unsigned int col, char dir, bool echoAll )
{
    wordPlay bestPlay;
    bool anyFound = false;// output control
    if( !pTileBoard[row][col] ) return bestPlay;// no tile to cross
    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;

    if( Midx >= wdLen ) return bestPlay;
    std::ifstream fin;//( fileName.c_str() );

    if( wdLen > 1 + rpHand.size() ) return bestPlay;// not enough tiles in hand
    if( !openDictFile( fin, wdLen ) ) return bestPlay;

    char Mltr = pTileBoard[row][col]->ltr;
    std::string fileWord;
    while( fin >> fileWord )
    {
        if( fileWord[Midx] == Mltr )// candidate
        {
            unsigned int wordScore = pTileBoard[row][col]->value, scoreFactor = 1;
            std::vector<bool> checkList( rpHand.size(), true );// all tiles held
            unsigned int i = 0;// which letter in word
            for( ; i < wdLen; ++i )
            {
           //     if( i == Midx ) { wordScore += pTileBoard[row][col]->value; continue; }
                if( i == Midx ) continue;
                unsigned int r = dir == 'H' ? row : row - Midx + i;
                unsigned int c = dir == 'V' ? col : col - Midx + i;
                unsigned int j = 0;// which tile in hand
                for( j = 0; j < rpHand.size(); ++j ) if( rpHand[j]->ltr == fileWord[i] && checkList[j] )
                {
                    checkList[j] = false;
                    wordScore += rpHand[j]->value;
                    if( spaceType[r][c] == 'D' ) scoreFactor *= 2;
                    else if( spaceType[r][c] == 'T' ) scoreFactor *= 3;
                    else if( spaceType[r][c] == 'd' ) wordScore += rpHand[j]->value;
                    else if( spaceType[r][c] == 't' ) wordScore += 2*rpHand[j]->value;
                    break;
                }

                if( j == rpHand.size() )// is a blank tile held?
                {
                    unsigned int k = 0;
                    for( k = 0; k < rpHand.size(); ++k )
                        if( rpHand[k]->ltr == ' ' && checkList[k] )
                        {
                            checkList[k] = false;
                            break;
                        }
                    if( k == rpHand.size() )break;// blank not found either
                }
            }

            if( i == wdLen )// can play this word
            {
                wordScore *= scoreFactor;
                if( wordScore > bestPlay.score )// new best
                    bestPlay.init( row, col, Midx, wordScore, fileWord, dir );

                if( !anyFound )
                {
                    if( echoAll )
                    {
                        for( unsigned int j = 0; j < wdLen; ++j ) std::cout << ( j==Midx ? Mltr : 'x' );
                        std::cout << ": ";
                    }
                    anyFound = true;
                }
                if( echoAll ) std::cout << "  " << fileWord << " score: " << wordScore;
            }

        }
    }

    if( echoAll && anyFound ) std::cout << '\n';

     return bestPlay;
}
*/

void lvl_scrabble::makeHighPlay()
{
    wordPlay& hp = highPlay;
    if( !pTileBoard[hp.row][hp.col] ) return;// no tile to cross
    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;
    unsigned int legIdx = 0;//, i = 0;
 //   unsigned int* pLegIdx = &legIdx;
 //   Tile* pt = nullptr;
    std::function<void(Tile*,unsigned int)> pFunc = [this,&legIdx,hp](Tile* pt,unsigned int i)
    {
        sf::Vector2f fPos = pTileBoard[hp.row][hp.col]->getPosition();
        if( hp.dir == 'H' ) fPos.x += i*Tile::tileSz.x - hp.Midx*Tile::tileSz.x;
        else fPos.y += i*Tile::tileSz.y - hp.Midx*Tile::tileSz.y;
        if( pt->ltr == ' ' ) { pt->ltr = hp.word[i]; pt->ltrTxt.setString( hp.word[i] ); std::cout << " MHP: with blank = " << hp.word[i]; }
        legAdaptVec[legIdx].init( playerLegVec[legIdx], [pt]( float x, float y ) { pt->setPosition(x,y); }, pt->getPosition().x, pt->getPosition().y, fPos.x, fPos.y, velStrip.fVal, accStrip.fVal, legIdx*delayStrip.fVal );
        ++legIdx;
    };
    hp.verify( rpHand, pFunc );

    anyMoving = true;
}

/*
void lvl_scrabble::makeHighPlay()
{
    wordPlay& hp = highPlay;
    if( !pTileBoard[hp.row][hp.col] ) return;// no tile to cross
    std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;
    unsigned int legIdx = 0;
    std::vector<bool> checkList( rpHand.size(), true );// all tiles held

    for( unsigned int i = 0; i < hp.word.length(); ++i )
    {
        if( i == hp.Midx ) continue;// existing letter

        sf::Vector2f fPos = pTileBoard[hp.row][hp.col]->getPosition();
        if( hp.dir == 'H' ) fPos.x += i*Tile::tileSz.x - hp.Midx*Tile::tileSz.x;
        else fPos.y += i*Tile::tileSz.y - hp.Midx*Tile::tileSz.y;

        // find ltr in hand
        unsigned int j = 0;
        for( j = 0; j < rpHand.size(); ++j )
        {
            Tile* pt = rpHand[j];//
            if( pt && pt->ltr == hp.word[i] && checkList[j] )
            {
                checkList[j] = false;// tile used
                legAdaptVec[legIdx].init( playerLegVec[legIdx], [pt]( float x, float y ) { pt->setPosition(x,y); }, pt->getPosition().x, pt->getPosition().y, fPos.x, fPos.y, velStrip.fVal, accStrip.fVal, legIdx*delayStrip.fVal );
                ++legIdx;
                break;
            }
        }

        if( j == rpHand.size() )// is a blank tile held?
        {
            unsigned int k = 0;
            for( k = 0; k < rpHand.size(); ++k )
            {
                Tile* pt = rpHand[k];
                if( pt->ltr == ' ' && checkList[k] )
                {
                    checkList[k] = false;
                    pt->ltr = hp.word[i]; pt->ltrTxt.setString( hp.word[i] ); std::cout << " with blank = " << hp.word[i];
                    legAdaptVec[legIdx].init( playerLegVec[legIdx], [pt]( float x, float y ) { pt->setPosition(x,y); }, pt->getPosition().x, pt->getPosition().y, fPos.x, fPos.y, velStrip.fVal, accStrip.fVal, legIdx*delayStrip.fVal );
                    ++legIdx;
                    break;
                }
            }
            if( k == rpHand.size() )break;// blank not found either
        }

    }
    anyMoving = true;
}
*/

void lvl_scrabble::saveGame() const
{
    std::ofstream fout("include/levels/scrabble/savedGame.txt");
    // basic datum
    fout << poolIdx << ' ' << playerScore << ' ' << pcScore << ' ' << ( playersTurn ? '1' : '0' ) << '\n';
    // write entire pool
    fout << TileVec.size() << ' ';
    for( const Tile tl : TileVec ) fout << ( tl.ltr == ' ' ? 'b' : tl.ltr );
    // write hands
    unsigned int handSz = 0;
    for( const Tile* pt : pTilesPlayer ) if( pt ) ++handSz;// # of tiles in hand
    fout << '\n' << handSz << ' ';
    for( const Tile* pt : pTilesPlayer ) if( pt ) fout << (int)( pt - &TileVec[0] ) << ' ';// offset into TileVec
    handSz = 0;
    for( const Tile* pt : pTilesPC ) if( pt ) ++handSz;// # of tiles in hand
    fout << '\n' << handSz << ' ';
    for( const Tile* pt : pTilesPC ) if( pt ) fout << (int)( pt - &TileVec[0] ) << ' ';// offset into TileVec
    // write board
    handSz = 0;// recycle
    for( unsigned int i = 0; i < 15*15; ++i ) if( pTileBoard[i/15][i%15] ) ++handSz;
    fout << '\n' << handSz << ' ';
    for( unsigned int i = 0; i < 15*15; ++i )
        if( pTileBoard[i/15][i%15] ) fout << ' ' << (int)( pTileBoard[i/15][i%15] - &TileVec[0] ) << ' ' << i/15 << ' ' << i%15;// offset + row + col
}

bool lvl_scrabble::loadSavedGame()
{
    std::ifstream fin("include/levels/scrabble/savedGame.txt");
    if( !fin ) return false;

    // basic datum
    fin >> poolIdx >> playerScore >> pcScore >> playersTurn;
    turnButt.setSel( playersTurn );
    if( playersTurn ) { playersTileArea.setFillColor( sf::Color::Cyan ); pcTileArea.setFillColor( sf::Color(100,100,100) ); }
    else { playersTileArea.setFillColor( sf::Color(100,100,100) ); pcTileArea.setFillColor( sf::Color::Cyan ); }
    sf::Text scrMsg;
    to_SF_string( scrMsg, playerScore );
    playScrMsg.setString( "Player: " + scrMsg.getString() );
    to_SF_string( scrMsg, pcScore );
    pcScrMsg.setString( "PC:     " + scrMsg.getString() );
    // write entire pool
    unsigned int poolSz; fin >> poolSz;
    std::cout << "\npoolSz = " << poolSz;
    TileVec.resize( poolSz );
    for( Tile& tl : TileVec )
    {
        char ltr;
        unsigned int val;
        fin >> ltr;
        if( ltr == 'b' ) { ltr = ' '; val = 0; }
        else val = ltrValue[ ltr - 'A' ];
        tl.init( ltr, val );
        std::cout << tl.ltr;
    }
    // read hands
    unsigned int handSz = 0, ofst = 0; fin >> handSz;
    pTilesPlayer.resize(7);
    std::cout << "\nhandSz = " << handSz;
    for( Tile*& pt : pTilesPlayer ) { if( handSz > 0 ){ --handSz; fin >> ofst; pt = &TileVec[ofst]; } else pt = nullptr; }
    std::cout << "\nplayers hand: ";
    for( unsigned int i = 0; i < pTilesPlayer.size(); ++i )
        if( pTilesPlayer[i] )
            {
                pTilesPlayer[i]->setPosition( playersTileArea.getPosition() + 0.5f*Tile::tileSz + sf::Vector2f( i*Tile::tileSz.x, 0.0f ) );
                std::cout << pTilesPlayer[i]->ltr;
            }
    fin >> handSz;
    pTilesPC.resize(7);
    std::cout << "\nhandSz = " << handSz;
    for( Tile*& pt : pTilesPC ) { if( handSz > 0 ){ --handSz; fin >> ofst; pt = &TileVec[ofst]; } else pt = nullptr; }
    std::cout << "\npc hand: ";
    for( unsigned int i = 0; i < pTilesPC.size(); ++i )
        if( pTilesPC[i] )
        {
            pTilesPC[i]->setPosition( pcTileArea.getPosition() + 0.5f*Tile::tileSz + sf::Vector2f( 0.0f, i*Tile::tileSz.y ) );
            std::cout << pTilesPC[i]->ltr;
        }

    // read board
    unsigned int row, col;
    fin >> handSz;
    std::cout << "\ntiles on board = " << handSz << '\n';
    for( unsigned int i = 0; i < 15*15; ++i ) pTileBoard[i/15][i%15] = nullptr;
    for( unsigned int i = 0; i < handSz; ++i )
    {
        fin >> ofst >> row >> col;
        pTileBoard[row][col] = &TileVec[ofst];
        TileVec[ofst].setPosition( boardPos + 0.5f*Tile::tileSz + sf::Vector2f( col*Tile::tileSz.x, row*Tile::tileSz.y ) );
        std::cout << pTileBoard[row][col]->ltr;
    }

    return true;
}


// non members
bool openDictFile( std::ifstream& fin, unsigned int wdLen )// return fin open to list of words of given length
{
    std::string fileName = "include/levels/scrabble/wordLists/";
    switch( wdLen )
    {
        case 2: fileName += "words2.txt"; break;
        case 3: fileName += "words3.txt"; break;
        case 4: fileName += "words4.txt"; break;
        case 5: fileName += "words5.txt"; break;
        case 6: fileName += "words6.txt"; break;
        case 7: fileName += "words7.txt"; break;
        case 8: fileName += "words8.txt"; break;
        case 9: fileName += "words9.txt"; break;
        case 10: fileName += "words10.txt"; break;
        return false;
    }

    fin.open( fileName.c_str() );
    return fin.good();
}

bool wordInList( const std::string& word )
{
    std::ifstream fin;//( fileName.c_str() );
    if( !openDictFile( fin, word.length() ) ) return false;
    std::string fileWord;
    while( fin >> fileWord )
        if( fileWord == word ) return true;

    return false;
}

void handReport( const std::vector<Tile*>& rpHand )
{
    std::cout << "\nhand: ";
    for( const Tile* pt : rpHand ) std::cout << pt->ltr << pt->turnsHeld << ' ';
    std::cout << '\n';
}

/*
void lvl_scrabble::dealTiles( float dt )
{
    static float tRemain = 0.0f;
    static unsigned int hiIdx = 0;

    if( hiIdx < legAdaptVec.size() )
    {
        tRemain -= dt;
        if( tRemain <= 0.0f )
        {
            ++hiIdx;
            tRemain = dealDelay;
        }
    }

    if( hiIdx == legAdaptVec.size() ) anyMoving = false;
    for( unsigned int i = 0; i < hiIdx; ++i )
    {
        legAdaptVec[i].update( dt );
        if( legAdaptVec[i].pLeg ) anyMoving = true;
    }

    // reset conditions
    if( !anyMoving )
    {
        hiIdx = 0;
        tRemain = 0.0f;
        return;
    }
}

void lvl_scrabble::castTiles( float dt )
{
    if( !anyMoving ) return;
    anyMoving = false;

    for( unsigned int i = 0; i < legAdaptVec.size(); ++i )
    {
        legAdaptVec[i].update( dt );
        if( legAdaptVec[i].pLeg ) anyMoving = true;
    }
}
*/

/*
// from words in file
void findPlays( char Mltr, unsigned int Midx, unsigned int wdLen, std::vector<Tile*>& rpHand )
{
    if( Midx >= wdLen ) return;
    std::ifstream fin;//( fileName.c_str() );

 //   std::vector<Tile*>& rpHand = playersTurn ? pTilesPlayer : pTilesPC;
    if( wdLen > 1 + rpHand.size() ) return;// not enough tiles in hand
    if( !openDictFile( fin, wdLen ) ) return;
 //   std::vector<bool> checkList( rpHand.size(), true );// all tiles held

    std::string fileWord;
    while( fin >> fileWord )
    {
        if( fileWord[Midx] == Mltr )// candidate
        {
            std::vector<bool> checkList( rpHand.size(), true );// all tiles held
            unsigned int i = 0;// which letter in word
            for( ; i < wdLen; ++i )
            {
                if( i == Midx ) continue;
            //    for( const Tile* ptl : rpHand )
                unsigned int j = 0;// which tile in hand
                for( j = 0; j < rpHand.size(); ++j ) if( rpHand[j]->ltr == fileWord[i] && checkList[j] )
                { checkList[j] = false; break; }

                if( j == rpHand.size() ) break;// letter not found in hand
            }

            if( i == wdLen )// can play this word
            {
                std::cout << "  " << fileWord;
            }

         //   std::cout << "  " << fileWord;
        }
    }
}

void showWordTemplates( char Mltr, unsigned int minX, unsigned int maxX, unsigned int X, std::vector<Tile*>& rpHand )// X is index to match letter
{
    X -= minX;// now minX is 0
    maxX -= minX;// word space length
    std::string word;
    unsigned int wdLenMax = maxX+1 < 8 ? maxX+1 : 8;
    std::cout << "\n** wdLenMax = " << wdLenMax << '\n';

    for( unsigned int wdLen = 2; wdLen <= wdLenMax; ++wdLen )
    {
        word.resize( wdLen );
        unsigned int Midx0 = ( wdLen < X + 1 ) ? wdLen-1 : X;
        unsigned int MidxF = ( maxX - X < wdLen - 1 ) ? wdLen-1 - (maxX-X) : 0;

        std::cout << "wdLen = " << wdLen << '\n';
        for( unsigned int i = MidxF; i <= Midx0; ++i )// each M position
        {
            for( unsigned int x = 0; x <= wdLen; ++x )
                if( x == i ) word[x] = Mltr;
                else word[x] = 'x';

         //   std::cout << word << '\n';
            std::cout << word;
            findPlays( Mltr, i, wdLen, rpHand );
            std::cout << '\n';
        }
    }
}
*/
