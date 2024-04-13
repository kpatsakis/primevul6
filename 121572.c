int encode_ac_prg_fs( abitwriter* huffw, huffCodes* actbl, short* block, unsigned int* eobrun, int from, int to )
{
    unsigned short n;
    unsigned char  s;
    unsigned char  z;
    int bpos;
    int hc;
    int tmp;

    // encode AC
    z = 0;
    for ( bpos = from; bpos <= to; bpos++ )
    {
        // if nonzero is encountered
        tmp = block[ bpos ];
        if ( tmp != 0 ) {
            // encode eobrun
            encode_eobrun( huffw, actbl, eobrun );
            // write remaining zeroes
            while ( z >= 16 ) {
                huffw->write( actbl->cval[ 0xF0 ], actbl->clen[ 0xF0 ] );
                z -= 16;
            }
            // vli encode
            s = nonzero_bit_length(ABS(tmp));
            n = ENVLI( s, tmp);
            hc = ( ( z << 4 ) + s );
            // write to huffman writer
            huffw->write( actbl->cval[ hc ], actbl->clen[ hc ] );
            huffw->write( n, s );
            // reset zeroes
            z = 0;
        }
        else { // increment zero counter
            z++;
        }
    }

    // check eob, increment eobrun if needed
    if ( z > 0 ) {
        (*eobrun)++;
        // check eobrun, encode if needed
        if ( (*eobrun) == actbl->max_eobrun )
            encode_eobrun( huffw, actbl, eobrun );
        return 1 + to - z;
    }
    else {
        return 1 + to;
    }
}