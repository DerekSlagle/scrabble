#ifndef LVL_SCRABBLE_H_INCLUDED
#define LVL_SCRABBLE_H_INCLUDED

#include "Level.h"
#include "../leg_types/linLeg.h"
#include "../leg_types/legAdapter.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonImg.h"
#include "../button_types/buttonTri.h"
#include "../button_types/keyPad.h"
#include "../spriteSheet.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

class Tile
{
    public:
    static sf::Vector2f tileSz;
    static sf::Color tileColor;
    const static sf::Font* pFont;
    static unsigned int ltrSz, valueSz;
    static sf::Vector2f ltrOfst, valueOfst;

    sf::RectangleShape rect;
    char ltr = 'A';
    unsigned int value = 1;
    unsigned int turnsHeld = 0;
    sf::Text ltrTxt, valueTxt;

    Tile(){}
    void init( char Ltr, unsigned int Value );
    Tile( char Ltr, unsigned int Value ) { init( Ltr, Value ); }
    void draw( sf::RenderTarget& RT ) const;
    void setPosition( sf::Vector2f Pos );
    void setPosition( float x, float y ) { setPosition( sf::Vector2f(x,y) ); }
    sf::Vector2f getPosition()const { return rect.getPosition(); }
    bool hit( sf::Vector2f hitPos ) const;
    bool hit( float x, float y ) const { return hit( sf::Vector2f(x,y) ); }
};

struct wordPlay
{
    unsigned int row=0, col=0, Midx=0, score=0;
    char dir = 'H';// or 'V'
    std::string word;
  //  std::vector<unsigned int> tileIdxVec;
    void init( unsigned int Row, unsigned int Col, unsigned int M_idx, unsigned int Score, const std::string Word, char Dir )
    { row = Row; col = Col; Midx = M_idx; score = Score; word = Word; dir = Dir; }
  //  bool verify( const std::vector<Tile*>& rpHand ) const;
    // process actions on each tile in play as well
    bool verify( const std::vector<Tile*>& rpHand, std::function<void(Tile*,unsigned int)> pFunc = nullptr ) const;
};

class lvl_scrabble : public Level
{
    public:

    static unsigned int poolIdx;// incremented as tiles are dealt
    std::vector<Tile> TileVec;// the tile pool
 //   std::vector<Tile> playerHand, pcHand;
    std::vector<Tile*> pTilesPlayer, pTilesPC;// the tile hands
 //   std::vector<linLeg> linLegVec;
    std::vector<linLeg> playerLegVec, pcLegVec;// animation paths
    std::vector<legAdapter> legAdaptVec;// adapt Tile to Leg for motion
    unsigned int playerScore = 0, pcScore = 0;
    sf::Text playScrMsg, pcScrMsg;// score display
    // some ui
    buttonRect startButt;// deal from pool
    buttonRect turnButt;// toggle turn
    buttonRect playPassButt;// trade tiles if selected, play otherwise
    buttonRect saveButt, loadSavedButt;// game save/load
    buttonRect findWordButt;// from letters placed on board. Also trigerrs tile deal from pool to replace those played
    buttonRect bestPlayButt;// find the best play

    bool anyMoving = false;// true if tile animation occuring
    bool playersTurn = true;
    float dealDelay = 16.0f, dealVel = 12.0f;
    // control some tile animation params
    buttonValOnHit delayStrip, velStrip, accStrip;
    controlSurface tileAniControl;

    // select tile - for trade or board placement
    sf::RectangleShape selRect;// to shade over tile selected for placement or trade
    Tile* pSelTile = nullptr;
    sf::RectangleShape showPlayRect;// to shade over word play bounds in showPlayBounds()
    bool showPlay = false;// draw above
    keyPad blankButt;// assign letter to blank tile
    Tile* pBlankTile = nullptr;// used by keyPad
    sf::Texture lettersTxt;

    // the board
    sf::Image boardImg;
    sf::Texture boardTxt;
    sf::Sprite boardSpr;
    sf::Vector2f boardPos;
    sf::Vector2f wndWSizeFactor;// tile sizing
    const Tile* pTileBoard[15][15];// played tiles recorded here
    char spaceType[15][15];// 's'=single, 'd'=double letter, 't'=triple letter, 'D'=double word, 'T'=triple word
    unsigned int ltrValue[26];// for tiles

    // players hand area
    sf::RectangleShape playersTileArea, pcTileArea;
    sf::RectangleShape passArea;// drawn when playPassButt.sel

    // other display
    sf::Text wordPlayedMsg, wordScoreMsg;
    wordPlay highPlay;// found during play search

    // other UI



    // functions
    lvl_scrabble():Level() { std::cout << "Hello from lvl_scrabble ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );// { return true; }
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void cleanup() {}
    virtual ~lvl_scrabble() { cleanup(); }

    void startTileDeal( std::vector<Tile*>& pHand, const sf::RectangleShape handArea, std::vector<linLeg>& legVec );// from pool to hand

    bool createBoard();// on init()
    void startTile_toBoard();// selection and placemnt
    void startTile_toHand();// return tile to hand
    void setWord();// submit and process play
    void tradeTiles();// player selected tiles
    void showPlayBounds( unsigned int row, unsigned int col, bool echoAll = true );// word play bounds across given tile
    bool hitBoard( sf::Vector2f hitPt, unsigned int& row, unsigned int& col ) const;

    // iterate over play templates eg. xxMxx, xxMx xMxx, xxM xMx Mxx, xM Mx. Call findPlays with each eg. for xxMx : Midx = 2, wdLen = 4
    void findWordTemplates( unsigned int minX, unsigned int maxX, unsigned int row, unsigned int col, char dir, bool echoAll = true );
    wordPlay find_plays( unsigned int Midx, unsigned int wdLen, unsigned int row, unsigned int col, char dir, bool echoAll = true );// returns best found
    void makeHighPlay();

    void saveGame() const;
    bool loadSavedGame();
};

bool openDictFile( std::ifstream& fin, unsigned int wdLen );// return fin open to list of words of given length
bool wordInList( const std::string& word );
void handReport( const std::vector<Tile*>& rpHand );


#endif // LVL_SCRABBLE_H_INCLUDED
