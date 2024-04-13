static int MP4_ReadBox_meta( stream_t *p_stream, MP4_Box_t *p_box )
{
    uint8_t meta_data[8];
    int i_actually_read;

    // skip over box header
    i_actually_read = stream_Read( p_stream, meta_data, 8 );
    if( i_actually_read < 8 )
        return 0;

    if ( p_box->p_father && p_box->p_father->i_type == ATOM_udta ) /* itunes udta/meta */
    {
        /* meta content starts with a 4 byte version/flags value (should be 0) */
        i_actually_read = stream_Read( p_stream, meta_data, 4 );
        if( i_actually_read < 4 || memcmp( meta_data, "\0\0\0", 4 ) )
            return 0;
    }

    if ( !MP4_ReadBoxContainerChildren( p_stream, p_box, ATOM_hdlr ) )
        return 0;

    /* Mandatory */
    const MP4_Box_t *p_hdlr = MP4_BoxGet( p_box, "hdlr" );
    if ( !p_hdlr || !BOXDATA(p_hdlr) ||
         ( BOXDATA(p_hdlr)->i_handler_type != HANDLER_mdta &&
           BOXDATA(p_hdlr)->i_handler_type != HANDLER_mdir ) ||
         BOXDATA(p_hdlr)->i_version != 0 )
        return 0;

    /* then it behaves like a container */
    return MP4_ReadBoxContainerRaw( p_stream, p_box );
}