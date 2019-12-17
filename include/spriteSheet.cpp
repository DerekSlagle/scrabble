#include "spriteSheet.h"

spriteSheet::spriteSheet( const char* fName )
{
    std::ifstream inFile( fName );
    if( inFile ) init(inFile);
}

spriteSheet::spriteSheet( std::ifstream& inFile ) { init(inFile); }

void spriteSheet::init( std::ifstream& inFile )
{
    getline( inFile, imgFname );

    img.loadFromFile( imgFname.c_str());
    txt.loadFromImage( img );
    size_t numSets;
    inFile >> numSets;
 //   std::cout << "\nnumSets read: " << numSets;
    frSetVec.reserve(numSets);

    size_t numFrames = 0;// read in a frame set
//    for( size_t i = 0; i < numSets && inFile >> numFrames; ++i )
    for( size_t i = 0; i < numSets; ++i )
    {
        inFile >> numFrames;
 //       std::cout << "\nnumFrames read: " << numFrames;
        std::vector<sf::IntRect> setVec;
        setVec.reserve(numFrames);
        int Lt, Tp, W, H;
        for( size_t j = 0; j < numFrames; ++j )
        {
            inFile >> Lt >> Tp >> W >> H;
     //       std::cout << "\nLt: " << Lt << " Tp: " << Tp << " W: " << W << " H: " << H;
            sf::IntRect temp(Lt,Tp,W,H);
            setVec.push_back(temp);
        }

        frSetVec.push_back( setVec );
  //      std::cout << "\nfrSetVec.size(): " << frSetVec.size();
    }

 //   std::cout << "\nfrSetVec.size(): " << frSetVec.size();
}

spriteSheet::~spriteSheet()
{
    //dtor
}

sf::IntRect spriteSheet::getFrRect( size_t& frIdx, size_t setNum, bool frAdvance ) const
{
    if( setNum >= frSetVec.size() ) setNum = frSetVec.size() - 1;
    if( frAdvance && ++frIdx >= frSetVec[setNum].size() ) frIdx = 0;
    return frSetVec[setNum][frIdx];
}

void spriteSheet::copySet( sf::RectangleShape* pR, size_t setNum )const
{
    if( setNum >= frSetVec.size() ) return;

    for( size_t i=0; i<frSetVec[setNum].size(); ++i )
    {
        sf::IntRect frRect = getFrRect(i,setNum);
        sf::Vector2f Rsz( frRect.width, frRect.height );
        pR[i].setSize( Rsz );
    //    pR[i].setTexture( &img );
        pR[i].setTexture( &txt );
        pR[i].setTextureRect( frRect );
    }
}

bool spriteSheet::toFile( const char* fName )const
{
    std::ofstream fout( fName );
    if( !fout ) return false;

    fout << imgFname << '\n';
    fout << '\n' << frSetVec.size() << '\n';// number of sets

    for( size_t i=0; i<frSetVec.size(); ++i )// for each frame set
    {
        fout << '\n' << frSetVec[i].size();// number of frames
        for( size_t j=0; j<frSetVec[i].size(); ++j )// for each frame
            fout << '\n' << getFrRect( j, i ).left << ' ' << getFrRect( j, i ).top << ' ' << getFrRect( j, i ).width << ' ' << getFrRect( j, i ).height;
    }

    fout.close();
    return true;
}

void quadPack::init( std::ifstream& fin, spriteSheet* p_SS )
{
    pSS = p_SS;

    size_t numLayers = 0, setNum = 0;
    float scale = 1.0f;
    fin >> scale;
    fin >> numLayers >> setNum;
    for( size_t i=0; i<numLayers; ++i )
    {
        float x0, y0; fin >> x0 >> y0;// start position
        if( i == 0 ) { pos.x = x0; pos.y = y0; }

        char dir = 'n';// direction to expand layer in: U,D,L,R
        size_t n; fin >> n >> dir;// number this layer
        float x = x0;
        sf::Vertex vt;
        for( size_t j=0; j<n; ++j )// each quad in this layer
        {
            size_t frIdx; fin >> frIdx;// area rendered
            sf::IntRect frRect = pSS->getFrRect( frIdx, setNum );

            vt.position.x = x;// up lt
            vt.position.y = y0;
            vt.texCoords.x = frRect.left;
            vt.texCoords.y = frRect.top;
            vtxVec.push_back( vt );
            vt.position.x = x + frRect.width*scale;// tp rt
            vt.position.y = y0;
            vt.texCoords.x = frRect.left + frRect.width;
            vt.texCoords.y = frRect.top;
        //    if(  dir == 'R' && i == 0 && j+1 == n ){ L.x = vt.position.x; L.y = vt.position.y; L -= pos; }
            if(  dir == 'R' && i == 0 && j+1 == n ){ L = vt.position; L -= pos; }
            vtxVec.push_back( vt );
            vt.position.x = x + frRect.width*scale;// bt rt
            vt.position.y = y0 + frRect.height*scale;
            vt.texCoords.x = frRect.left + frRect.width;
            vt.texCoords.y = frRect.top + frRect.height;
       //     if(  dir == 'D' && i == 0 && j+1 == n ){ L.x = vt.position.x; L.y = vt.position.y; L -= pos; }
            if(  dir == 'D' && i == 0 && j+1 == n ){ L = vt.position; L -= pos; }
            vtxVec.push_back( vt );
            vt.position.x = x;// bt lt
            vt.position.y = y0 + frRect.height*scale;
            vt.texCoords.x = frRect.left;
            vt.texCoords.y = frRect.top + frRect.height;
            vtxVec.push_back( vt );

            if( dir == 'R' ) x += frRect.width*scale;
            else if( dir == 'D' ) y0 += frRect.height*scale;
        }
    }
 //   std::cerr << "quadPack.init(): L = " << L.mag() << '\n';
//    std::cerr << "quadPack.init(): L = " << L.mag() << '\n';
}
