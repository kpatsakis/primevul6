int encode_dc_prg_sa( abitwriter* huffw, short* block )
{
    // enocode next bit of dc coefficient
    huffw->write( block[ 0 ], 1 );

    // return 0 if everything is ok
    return 0;
}