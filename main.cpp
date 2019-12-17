// *************************
// copyright(c) 2018 Derek Slagle
//
// ************************

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <fstream>
#include <algorithm>
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
//#include "include/button_types/buttonRect.h"
//#include "include/leg_types/linLeg.h"
//#include "include/level/Level.h"
#include "include/levels/lvl_scrabble.h"
#include "include/levels/lvlButts.h"
#include "include/levels/lvl_truck.h"
#include "include/levels/lvl_paths.h"
#include "include/levels/lvl_squads.h"
#include "include/levels/lvl_perspect.h"
#include "include/levels/lvl_track.h"
#include "include/levels/menu.h"

unsigned int wdwW = 1280, wdwH = 720;// window dimensions
//unsigned int wdwW = 1600, wdwH = 900;// window dimensions
float Level::winW = (float)wdwW;
float Level::winH = (float)wdwH;

Level* loadLevel( size_t I );

int main()
{
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return EXIT_FAILURE;
    button::init( font, 30 );
    Tile::pFont = &font;

    menu main_Menu;
    if( !main_Menu.init() ) { std::cout << "main menu init fail\n"; return 1; }
    main_Menu.pLoadLvl = loadLevel;
    Level::init_stat( &main_Menu );

    lvl_scrabble*  pTheGame = new lvl_scrabble;
    if( !pTheGame->init() ) { std::cout << "theGame init fail\n"; return 1; }
 //   Level::winW = Tile::tileSz.x*pTheGame->wndWSizeFactor.x;
 //   Level::winH = Tile::tileSz.y*pTheGame->wndWSizeFactor.y;

 //   Level::pCurrLvl = pTheGame;
    delete pTheGame;
    Level::pCurrLvl = &main_Menu;
    if( !Level::pCurrLvl ) return 2;

    wdwW = floor( abs( Level::winW ) );
    wdwH = floor( abs( Level::winH ) );

    // ready to go!
    sf::RenderWindow RW(sf::VideoMode(wdwW, wdwH), "Scrabble",  sf::Style::Titlebar | sf::Style::Close);
    RW.setVerticalSyncEnabled(true);
    float dt = 1.0f;

     while (RW.isOpen())
     {
        button::reset();

        sf::Event event;
        while (RW.pollEvent(event))
            if( !Level::handleEvent_stat( event ) )
            { RW.close(); break; }

        Level::update_stat(dt);

        // draw
        RW.clear( Level::clearColor );// sf::Color(0,64,128) )
        Level::draw_stat(RW);
        RW.display();

    /*    if( Level::do_capture )
        {
            Level::do_capture = false;
            sf::Image captImg = RW.capture();
            captImg.saveToFile( "screenCapt.png" );
        }   */
     }


    Level::cleanup_stat();

    return 0;
}

Level* loadLevel( size_t I )
{
    Level* pLvl = nullptr;

    switch( I )
    {
        case 0 :
        pLvl = new lvl_scrabble;
        break;

        case 1 :
        pLvl = new lvlButts;
        break;

        case 2 :
        pLvl = new lvl_truck;
        break;

        case 3 :
        pLvl = new lvl_paths;
        break;

        case 4 :
        pLvl = new lvl_squads;
        break;

        case 5 :
        pLvl = new lvl_perspect;
        break;

        case 6 :
        pLvl = new lvl_track;
        break;
    }

    if( !pLvl ) return nullptr;
    if( pLvl->init() ) return pLvl;
    else { delete pLvl; return nullptr; }
}

