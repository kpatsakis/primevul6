int decode_ac_prg_sa( abitreader* huffr, huffTree* actree, short* block, unsigned int* eobrun, int from, int to )
{
    unsigned short n;
    unsigned char  s;
    signed char    z;
    signed char    v;
    int bpos = from;
    int eob = to;
    int hc;
    int l;
    int r;


    // decode AC succesive approximation bits
    if ( (*eobrun) == 0 )
    while ( bpos <= to )
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
            if ( s == 0 ) v = 0;
            else if ( s == 1 ) {
                n = huffr->read( 1 );
                v = ( n == 0 ) ? -1 : 1; // fast decode vli
            }
            else return -1; // decoding error
            // write zeroes / write correction bits
            while ( true ) {
                if ( block[ bpos ] == 0 ) { // skip zeroes / write value
                    if ( z > 0 ) z--;
                    else {
                        block[ bpos++ ] = v;
                        break;
                    }
                }
                else { // read correction bit
                    n = huffr->read( 1 );
                    block[ bpos ] = ( block[ bpos ] > 0 ) ? n : -n;
                }
                if ( bpos++ >= to ) return -1; // error check            
            }
        }
        else { // decode eobrun
            eob = bpos;
            s = l;
            n = huffr->read( s );
            (*eobrun) = E_DEVLI( s, n );
            break;
        }
    }

    // read after eob correction bits
    if ( (*eobrun) > 0 ) {
        for ( ; bpos <= to; bpos++ ) {
            if ( block[ bpos ] != 0 ) {
                n = huffr->read( 1 );
                block[ bpos ] = ( block[ bpos ] > 0 ) ? n : -n;
            }
        }
        // decrement eobrun
        (*eobrun)--;
    }

    // return eob
    return eob;
}