#include "parBkgd.h"

parBkgd::parBkgd()
{
    //ctor
}

parBkgd::parBkgd( string config_fname )
{
    unsigned int N_scrImgs = 0, N_layers = 0;
    std::string fnameImg;

    std::ifstream fin( config_fname.c_str() );

    if( fin )
    {
        valid = true;
        std::getline( fin, fnameImg );
        if( !img.LoadFromFile( fnameImg.c_str() ) )
            valid = false;

        if( valid )
        {
            fin >> N_scrImgs >> N_layers;// cout << "N_scrImgs = " << N_scrImgs << "  N_layers = " << N_layers << endl;

            scrImgVec.reserve( N_scrImgs );
            p_layerVec.reserve( N_layers );
            layerTypeVec.reserve( N_scrImgs + N_layers );

            for(unsigned int i=0; i< (N_scrImgs + N_layers); ++i)
            {
                char layerType = 's';

                fin >> layerType;
                layerTypeVec.push_back( layerType );

                switch( layerType )
                {
                    case 's':// single strip layer
                    scrImgVec.push_back( scrollImage( img, fin ) );
                    break;

                    case 'e':// elements layer
                    p_layerVec.push_back( new BkgdArr( img, g_scale, fin ) );
                    break;

                    default:
                    break;
                }
            }// end for loop

            float vx = 0.0f;
            fin >> vx;
            setSpeed( vx );
        }

        fin.close();
    }
    else// could not open file
        valid = false;

//    cout << "file not opened" << endl;
}// end of ctor

parBkgd::~parBkgd()
{
    for(unsigned int i=0; i< p_layerVec.size(); ++i)
            if( p_layerVec[i] ) delete p_layerVec[i];
}

void parBkgd::move(void)
{
    unsigned int i=0;

    for( i=0; i< scrImgVec.size(); ++i) scrImgVec[i].move();
    for( i=0; i< p_layerVec.size(); ++i) p_layerVec[i]->move();

    return;
}

void parBkgd::draw(void)
{
    unsigned int i=0, s=0, e=0;

    for( i=0; i< layerTypeVec.size(); ++i)
    {
        if( layerTypeVec[i] == 's' && s < scrImgVec.size() )
            scrImgVec[s++].draw();
        else if( layerTypeVec[i] == 'e' && e < p_layerVec.size() )
            p_layerVec[e++]->draw();

    }

    return;
}

void parBkgd::setColor( const sf::Color& newColor )// used to fade out the entire background (all layers)
{
    unsigned int i=0;

    for( i=0; i< scrImgVec.size(); ++i) scrImgVec[i].setColor( newColor );
    for( i=0; i< p_layerVec.size(); ++i) p_layerVec[i]->setColor( newColor );

    return;
}

void parBkgd::setSpeed( float Vx )
{
    unsigned int i=0;

    for( i=0; i< scrImgVec.size(); ++i) scrImgVec[i].vx = Vx*scrImgVec[i].vFactor;
    for( i=0; i< p_layerVec.size(); ++i) p_layerVec[i]->vx = Vx*p_layerVec[i]->vFactor;

    return;
}
