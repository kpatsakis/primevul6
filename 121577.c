int decode_dc_prg_fs( abitreader* huffr, huffTree* dctree, short* block )
{
    unsigned short n;
    unsigned char  s;
    int hc;


    // decode dc
    hc = next_huffcode( huffr, dctree, Billing::EXP0_DC, Billing::EXPN_DC);
    if ( hc < 0 ) return -1; // return error
    else s = ( unsigned char ) hc;
    n = huffr->read( s );
    block[ 0 ] = DEVLI( s, n );


    // return 0 if everything is ok
    return 0;
}