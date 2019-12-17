#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include<string>
#include "Level.h"

class menu : public Level
{
    public:
    std::vector<sf::Text> msgVec;
    std::vector< sf::Rect<float> > rectVec;
    sf::Color msgClrReg;
    sf::Color msgClrMseOver;
    size_t numOptions = 0;
    size_t lvlNum = 0;
    std::function<Level*(size_t)> pLoadLvl = nullptr;

 //   okBox levelBox;

    // functions
    menu():Level() { std::cout << "Hello from menu ctor\n"; }

    virtual bool init();
    virtual void reset();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~menu();
};

#endif // MENU_H_INCLUDED
