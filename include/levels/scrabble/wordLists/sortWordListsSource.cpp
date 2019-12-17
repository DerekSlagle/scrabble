// sort words into lists by word length

#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::ifstream fin( "wordLists/wordsAll.txt" );
    if( !fin ) return 1;

    const unsigned int listCount = 7;
    std::ofstream fout[listCount];// for 2ltr thru 8ltr words
    unsigned int wdCount = 0, eachCount[listCount] = {0};
    fout[0].open( "wordLists/words2.txt" ); if( !fout[0].good() ) return 2;
    fout[1].open( "wordLists/words3.txt" ); if( !fout[1].good() ) return 3;
    fout[2].open( "wordLists/words4.txt" ); if( !fout[2].good() ) return 4;
    fout[3].open( "wordLists/words5.txt" ); if( !fout[3].good() ) return 5;
    fout[4].open( "wordLists/words6.txt" ); if( !fout[4].good() ) return 6;
    fout[5].open( "wordLists/words7.txt" ); if( !fout[5].good() ) return 7;
    fout[6].open( "wordLists/words8.txt" ); if( !fout[6].good() ) return 8;

    std::string wordIn;
    while( fin >> wordIn )
    {
        if( wordIn.length() < 2 || wordIn.length() > 1 + listCount ) continue;
        for( auto& ltr : wordIn ) ltr += 'A' - 'a';
        fout[ wordIn.length() - 2 ] << wordIn << ' ';
        ++eachCount[ wordIn.length() - 2 ];
    }

    // results
    std::ofstream foutRpt( "wordLists/listInfo.txt" );
    for( unsigned int i = 0; i < listCount; ++i )
    {
        std::cout << eachCount[i] << " " << i+2 << " letter words\n";
        foutRpt << eachCount[i] << " " << i+2 << " letter words\n";
        wdCount += eachCount[i];
    }

    std::cout << wdCount << " total words\n";
    foutRpt << wdCount << " total words\n";

    return 0;
}
