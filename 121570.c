int encode_crbits( abitwriter* huffw, abytewriter* storw )
{
    unsigned char* data;
    int len;
    int i;


    // peek into data from abytewriter
    len = storw->getpos();
    if ( len == 0 ) return 0;
    data = storw->peekptr_aligned();

    // write bits to huffwriter
    for ( i = 0; i < len; i++ )
        huffw->write( data[ i ], 1 );

    // reset abytewriter, discard data
    storw->reset();


    return 0;
}