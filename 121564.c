int encode_eobrun( abitwriter* huffw, huffCodes* actbl, unsigned int* eobrun )
{
    unsigned short n;
    unsigned int  s;
    int hc;


    if ( (*eobrun) > 0 ) {
        while ( (*eobrun) > actbl->max_eobrun ) {
            huffw->write( actbl->cval[ 0xE0 ], actbl->clen[ 0xE0 ] );
            huffw->write( E_ENVLI( 14, 32767 ), 14 );
            (*eobrun) -= actbl->max_eobrun;
        }
        s = uint16bit_length((*eobrun));
        dev_assert(s && "actbl->max_eobrun needs to be > 0");
        if (s) s--;
        n = E_ENVLI( s, (*eobrun) );
        hc = ( s << 4 );
        huffw->write( actbl->cval[ hc ], actbl->clen[ hc ] );
        huffw->write( n, s );
        (*eobrun) = 0;
    }


    return 0;
}