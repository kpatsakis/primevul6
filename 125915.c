static int MP4_ReadBox_pnot( stream_t *p_stream, MP4_Box_t *p_box )
{
    if ( p_box->i_size != 20 )
        return 0;
    MP4_READBOX_ENTER( MP4_Box_data_pnot_t );
    MP4_GET4BYTES( p_box->data.p_pnot->i_date );
    uint16_t i_version;
    MP4_GET2BYTES( i_version );
    if ( i_version != 0 )
        MP4_READBOX_EXIT( 0 );
    MP4_GETFOURCC( p_box->data.p_pnot->i_type );
    MP4_GET2BYTES( p_box->data.p_pnot->i_index );
    MP4_READBOX_EXIT( 1 );
}