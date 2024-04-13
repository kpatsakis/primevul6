int decode_ac_prg_fs( abitreader* huffr, huffTree* actree, short* block, unsigned int* eobrun, int from, int to )
{
    unsigned short n;
    unsigned char  s;
    unsigned char  z;
    int eob = to + 1;
    int bpos;
    int hc;
    int l;
    int r;


    // check eobrun
    if ( (*eobrun) > 0 ) {
        for ( bpos = from; bpos <= to; ++bpos)
            block[ bpos ] = 0;
        (*eobrun)--;
        return from;
    }

    // decode ac
    for ( bpos = from; bpos <= to; )
    {
        // decode next
        hc = next_huffcode( huffr, actree,
                            is_edge(bpos) ? Billing::BITMAP_EDGE : Billing::BITMAP_7x7,
                            is_edge(bpos) ? Billing::EXPN_EDGE : Billing::EXPN_7x7);
        if ( hc < 0 ) return -1;
        l = LBITS( hc, 4 );
        r = RBITS( hc, 4 );
        // analyse code
        if ( ( l == 15 ) || ( r > 0 ) ) { // decode run/level combination
            z = l;
            s = r;
            n = huffr->read( s );
            if ( ( z + bpos ) > to )
                return -1; // run is to long
            while ( z > 0 ) { // write zeroes
                block[ bpos++ ] = 0;
                z--;
            }
            block[ bpos++ ] = ( short ) DEVLI( s, n ); // decode cvli
        }
        else { // decode eobrun
            eob = bpos;
            s = l;
            n = huffr->read( s );
            (*eobrun) = E_DEVLI( s, n );
            // while( bpos <= to ) // fill remaining block with zeroes
            //    block[ bpos++ ] = 0;
            (*eobrun)--; // decrement eobrun ( for this one )
            break;
        }
    }


    // return position of eob
    return eob;
}