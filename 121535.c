bool rebuild_header_jpg( void )
{
    abytewriter* hdrw; // new header writer

    unsigned char  type = 0x00; // type of current marker segment
    uint32_t   len  = 0; // length of current marker segment
    uint32_t   hpos = 0; // position in header


    // start headerwriter
    hdrw = new abytewriter( 4096 );

    // header parser loop
    while ( hpos < hdrs && (uint64_t)hpos + 3 < (uint64_t)hdrs ) {
        type = hpos + 1 < hdrs ?  hdrdata[ hpos + 1 ] : 0;
        len = 2 + B_SHORT( hpos + 2 < hdrs ? hdrdata[ hpos + 2 ]:0, hpos + 3 < hdrs ? hdrdata[ hpos + 3 ] :0);
        // discard any unneeded meta info
        if ( ( type == 0xDA ) || ( type == 0xC4 ) || ( type == 0xDB ) ||
             ( type == 0xC0 ) || ( type == 0xC1 ) || ( type == 0xC2 ) ||
             ( type == 0xDD ) ) {
            uint32_t to_copy = hpos + len < hdrs ? len : hdrs - hpos;
            hdrw->write_n( &(hdrdata[ hpos ]), to_copy);
            if (to_copy <  len) {
                for (uint32_t i = 0;i <to_copy -len;++i) {
                    uint8_t zero = 0;
                    hdrw->write_n(&zero, 1);
                }
            }
        }
        hpos += len;
    }

    // replace current header with the new one
    custom_free( hdrdata );
    hdrdata = hdrw->getptr_aligned();
    hdrs    = hdrw->getpos();
    delete( hdrw );


    return true;
}