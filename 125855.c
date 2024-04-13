static int MP4_ReadBox_tsel( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_tsel_t );
    uint32_t i_version;
    MP4_GET4BYTES( i_version );
    if ( i_version != 0 || i_read < 4 )
        MP4_READBOX_EXIT( 0 );
    MP4_GET4BYTES( p_box->data.p_tsel->i_switch_group );
    /* ignore list of attributes as es are present before switch */
    MP4_READBOX_EXIT( 1 );
}