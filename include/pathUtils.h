#ifndef PATHUTILS_H_INCLUDED
#define PATHUTILS_H_INCLUDED

#include<vector>
#include<fstream>
#include<iostream>
#include<random>
#include <SFML/Graphics.hpp>
#include "leg_types/linLeg.h"
#include "leg_types/cirLeg.h"
#include "leg_types/flexLeg.h"
#include "leg_types/funcLeg.h"

void makePath( std::ifstream& inFile, Leg*& pLeg );
void makeFuncPath( std::ifstream& inFile, Leg*& pLeg );
void makeFlyPath_lin( std::ifstream& fin, Leg*& pLeg );
void makeFlyPath_cir( std::ifstream& fin, Leg*& pLeg );
void makeBlockPath( std::ifstream& fin, std::vector<Leg*>& pLegVec );// rows paths created
int destroyPath( Leg*& pLeg );// returns # of Legs deleted

void tracePath( Leg& rLeg, float ds, std::vector<sf::Vertex>& vtxVec, sf::Color reg, sf::Color join );
float pathLength( Leg& rLeg );

#endif // PATHUTILS_H_INCLUDED
