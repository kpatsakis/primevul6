bool reset_buffers( void )
{
    int cmp, bpos;
    int i;


    // -- free buffers --

    // free buffers & set pointers NULL
    if ( hdrdata  != NULL ) aligned_dealloc ( hdrdata );
    if ( huffdata != NULL ) aligned_dealloc ( huffdata );
    if ( grbgdata != NULL && grbgdata != EOI ) aligned_dealloc ( grbgdata );
    rst_err.clear();
    rstp.resize(0);
    scnp.resize(0);
    hdrdata   = NULL;
    huffdata  = NULL;
    grbgdata  = NULL;

    // free image arrays
    colldata.reset();


    // -- set variables --

    // preset componentinfo
    for ( cmp = 0; cmp < 4; cmp++ ) {
        cmpnfo[ cmp ].sfv = -1;
        cmpnfo[ cmp ].sfh = -1;
        cmpnfo[ cmp ].mbs = -1;
        cmpnfo[ cmp ].bcv = -1;
        cmpnfo[ cmp ].bch = -1;
        cmpnfo[ cmp ].bc  = -1;
        cmpnfo[ cmp ].ncv = -1;
        cmpnfo[ cmp ].nch = -1;
        cmpnfo[ cmp ].nc  = -1;
        cmpnfo[ cmp ].sid = -1;
        cmpnfo[ cmp ].jid = -1;
        cmpnfo[ cmp ].qtable = NULL;
        cmpnfo[ cmp ].huffdc = -1;
        cmpnfo[ cmp ].huffac = -1;
    }

    // preset imgwidth / imgheight / component count
    imgwidth  = 0;
    imgheight = 0;
    cmpc      = 0;

    // preset mcu info variables / restart interval
    sfhm      = 0;
    sfvm      = 0;
    mcuc      = 0;
    mcuh      = 0;
    mcuv      = 0;
    rsti      = 0;
    max_file_size = 0; // this file isn't truncated
    // reset quantization / huffman tables
    for ( i = 0; i < 4; i++ ) {
        htset[ 0 ][ i ] = 0;
        htset[ 1 ][ i ] = 0;
        for ( bpos = 0; bpos < 64; bpos++ )
            qtables[ i ][ bpos ] = 0;
    }

    // preset jpegtype
    jpegtype  = 0;

    // reset padbit
    padbit = -1;

    return true;
}