int decode_block_seq( abitreader* huffr, huffTree* dctree, huffTree* actree, short* block )
{
    unsigned short n;
    unsigned char  s;
    unsigned char  z;
    int eob = 64;
    int bpos;
    int hc;


    // decode dc
    hc = next_huffcode( huffr, dctree, Billing::EXP0_DC, Billing::EXPN_DC);
    if ( hc < 0 ) return -1; // return error
    else s = ( unsigned char ) hc;
    n = huffr->read( s );
    if (s) {
        write_bit_bill(Billing::RES_DC, false, s - 1);
        write_bit_bill(Billing::SIGN_DC, false, 1);
    }
    block[ 0 ] = DEVLI( s, n );
    bool eof_fixup = false;
    // decode ac
    for ( bpos = 1; bpos < 64; )
    {
        // decode next
        hc = next_huffcode( huffr, actree,
                            is_edge(bpos) ? Billing::BITMAP_EDGE : Billing::BITMAP_7x7,
                            is_edge(bpos) ? Billing::EXPN_EDGE : Billing::EXPN_7x7);
        // analyse code
        if ( hc > 0 ) {
            z = LBITS( hc, 4 );
            s = RBITS( hc, 4 );
            n = huffr->read( s );
            if (s) {
                write_bit_bill(is_edge(bpos) ? Billing::RES_EDGE : Billing::RES_7x7, false, s - 1);
                write_bit_bill(is_edge(bpos) ? Billing::SIGN_EDGE : Billing::SIGN_7x7, false, 1);
            }
            if ( ( z + bpos ) >= 64 ) {
                eof_fixup = true;
                break;
            }
            while ( z > 0 ) { // write zeroes
                block[ bpos++ ] = 0;
                z--;
            }
            block[ bpos++ ] = ( short ) DEVLI( s, n ); // decode cvli
        }
        else if ( hc == 0 ) { // EOB
            eob = bpos;
            // while( bpos < 64 ) // fill remaining block with zeroes
            //    block[ bpos++ ] = 0;
            break;
        }
        else {
            return -1; // return error
        }
    }
    if (eof_fixup) {
        always_assert(huffr->eof && "If 0run is longer than the block must be truncated");
        for(;bpos < eob; ++bpos) {
            block[bpos] = 0;
        }
        if (eob) {
            block[eob - 1] = 1; // set the value to something matching the EOB
        }
    }
    // return position of eob
    return eob;
}