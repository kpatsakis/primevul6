int decode_dc_prg_sa( abitreader* huffr, short* block )
{
    // decode next bit of dc coefficient
    block[ 0 ] = huffr->read( 1 );

    // return 0 if everything is ok
    return 0;
}