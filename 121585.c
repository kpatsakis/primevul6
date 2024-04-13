bool setup_imginfo_jpg(bool only_allocate_two_image_rows)
{
    unsigned char  type = 0x00; // type of current marker segment
    unsigned int   len  = 0; // length of current marker segment
    unsigned int   hpos = 0; // position in header

    int cmp;

    // header parser loop
    while ( hpos < hdrs ) {
        type = hpos + 1 < hdrs ? hdrdata[ hpos + 1 ] : 0;
        len = 2 + B_SHORT( hpos + 2 < hdrs ? hdrdata[ hpos + 2 ] : 0, hpos + 3 < hdrs ? hdrdata[ hpos + 3 ] : 0);
        // do not parse DHT & DRI
        if ( ( type != 0xDA ) && ( type != 0xC4 ) && ( type != 0xDD ) ) {
            if ( !parse_jfif_jpg( type, len, hdrs-hpos, &( hdrdata[ hpos ] ) ) )
                return false;
        }
        hpos += len;
    }

    // check if information is complete
    if ( cmpc == 0 ) {
        fprintf( stderr, "header contains incomplete information" );
        errorlevel.store(2);
        return false;
    }
    for ( cmp = 0; cmp < cmpc; cmp++ ) {
        if ( ( cmpnfo[cmp].sfv == 0 ) ||
             ( cmpnfo[cmp].sfh == 0 ) ||
             ( cmpnfo[cmp].qtable == NULL ) ||
             ( cmpnfo[cmp].qtable[0] == 0 ) ||
             ( jpegtype == 0 ) ) {
            fprintf( stderr, "header information is incomplete" );
            errorlevel.store(2);
            return false;
        }
    }
        
    // do all remaining component info calculations
    for ( cmp = 0; cmp < cmpc; cmp++ ) {
        if ( cmpnfo[ cmp ].sfh > sfhm ) sfhm = cmpnfo[ cmp ].sfh;
        if ( cmpnfo[ cmp ].sfv > sfvm ) sfvm = cmpnfo[ cmp ].sfv;
    }
    mcuv = ( int ) ceil( (float) imgheight / (float) ( 8 * sfhm ) );
    mcuh = ( int ) ceil( (float) imgwidth  / (float) ( 8 * sfvm ) );
    mcuc  = mcuv * mcuh;
    int maxChromaWidth = 0;
    int maxChromaHeight = 0;
    int maxLumaWidth = 0;
    int maxLumaHeight = 0;
    for ( cmp = 0; cmp < cmpc; cmp++ ) {
        cmpnfo[ cmp ].mbs = cmpnfo[ cmp ].sfv * cmpnfo[ cmp ].sfh;
        cmpnfo[ cmp ].bcv = mcuv * cmpnfo[ cmp ].sfh;
        cmpnfo[ cmp ].bch = mcuh * cmpnfo[ cmp ].sfv;
        cmpnfo[ cmp ].bc  = cmpnfo[ cmp ].bcv * cmpnfo[ cmp ].bch;
        cmpnfo[ cmp ].ncv = ( int ) ceil( (float) imgheight *
                            ( (float) cmpnfo[ cmp ].sfh / ( 8.0 * sfhm ) ) );
        cmpnfo[ cmp ].nch = ( int ) ceil( (float) imgwidth *
                            ( (float) cmpnfo[ cmp ].sfv / ( 8.0 * sfvm ) ) );
        cmpnfo[ cmp ].nc  = cmpnfo[ cmp ].ncv * cmpnfo[ cmp ].nch;
        cmpnfo[cmp].check_valid_value_range();
        if (cmp == 0) {
            maxLumaWidth = cmpnfo[ cmp ].bch * 8;
            maxLumaHeight = cmpnfo[ cmp ].bcv * 8;
        } else {
            if (maxChromaWidth < cmpnfo[ cmp ].bch * 8) {
                maxChromaWidth = cmpnfo[ cmp ].bch * 8;
            }
            if (maxChromaHeight < cmpnfo[ cmp ].bcv * 8) {
                maxChromaHeight = cmpnfo[ cmp ].bcv * 8;
            }
        }
    }
    LeptonDebug::setupDebugData(maxLumaWidth, maxLumaHeight,
                                maxChromaWidth, maxChromaHeight);

    // decide components' statistical ids
    if ( cmpc <= 3 ) {
        for ( cmp = 0; cmp < cmpc; cmp++ ) cmpnfo[ cmp ].sid = cmp;
    }
    else {
        for ( cmp = 0; cmp < cmpc; cmp++ ) cmpnfo[ cmp ].sid = 0;
    }
    size_t start_allocated = Sirikata::memmgr_size_allocated();
    // alloc memory for further operations
    colldata.init(cmpnfo, cmpc, mcuh, mcuv, jpegtype == 1 && only_allocate_two_image_rows);
    size_t end_allocated = Sirikata::memmgr_size_allocated();
    total_framebuffer_allocated = end_allocated - start_allocated;
    return true;
}