int encode_dc_prg_fs( abitwriter* huffw, huffCodes* dctbl, short* block )
{
    unsigned short n;
    unsigned char  s;
    int tmp;


    // encode DC
    tmp = block[ 0 ];
    s = uint16bit_length(ABS(tmp));
    n = ENVLI( s, tmp );
    huffw->write( dctbl->cval[ s ], dctbl->clen[ s ] );
    huffw->write( n, s );


    // return 0 if everything is ok
    return 0;
}