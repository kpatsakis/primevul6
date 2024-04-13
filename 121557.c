int encode_ac_prg_sa( abitwriter* huffw, abytewriter* storw, huffCodes* actbl, short* block, unsigned int* eobrun, int from, int to )
{
    unsigned short n;
    unsigned char  s;
    unsigned char  z;
    int eob = from;
    int bpos;
    int hc;
    int tmp;

    // check if block contains any newly nonzero coefficients and find out position of eob
    for ( bpos = to; bpos >= from; bpos-- )    {
        if ( ( block[ bpos ] == 1 ) || ( block[ bpos ] == -1 ) ) {
            eob = bpos + 1;
            break;
        }
    }

    // encode eobrun if needed
    if ( ( eob > from ) && ( (*eobrun) > 0 ) ) {
        encode_eobrun( huffw, actbl, eobrun );
        encode_crbits( huffw, storw );
    }

    // encode AC
    z = 0;
    for ( bpos = from; bpos < eob; bpos++ )
    {
        tmp = block[ bpos ];
        // if zero is encountered
        if ( tmp == 0 ) {
            z++; // increment zero counter
            if ( z == 16 ) { // write zeroes if needed
                huffw->write( actbl->cval[ 0xF0 ], actbl->clen[ 0xF0 ] );
                encode_crbits( huffw, storw );
                z = 0;
            }
        }
        // if nonzero is encountered
        else if ( ( tmp == 1 ) || ( tmp == -1 ) ) {
            // vli encode
            s = nonzero_bit_length(ABS(tmp));
            n = ENVLI( s, tmp );
            hc = ( ( z << 4 ) + s );
            // write to huffman writer
            huffw->write( actbl->cval[ hc ], actbl->clen[ hc ] );
            huffw->write( n, s );
            // write correction bits
            encode_crbits( huffw, storw );
            // reset zeroes
            z = 0;
        }
        else { // store correction bits
            n = block[ bpos ] & 0x1;
            storw->write( n );
        }
    }

    // fast processing after eob
    for ( ;bpos <= to; bpos++ )
    {
        if ( block[ bpos ] != 0 ) { // store correction bits
            n = block[ bpos ] & 0x1;
            storw->write( n );
        }
    }

    // check eob, increment eobrun if needed
    if ( eob <= to ) {
        (*eobrun)++;
        // check eobrun, encode if needed
        if ( (*eobrun) == actbl->max_eobrun ) {
            encode_eobrun( huffw, actbl, eobrun );
            encode_crbits( huffw, storw );
        }
    }

    // return eob
    return eob;
}