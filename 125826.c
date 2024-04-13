static int MP4_ReadBox_ASF( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_ASF_t );

    MP4_Box_data_ASF_t *p_asf = p_box->data.p_asf;

    if (i_read != 8)
        MP4_READBOX_EXIT( 0 );

    MP4_GET1BYTE( p_asf->i_stream_number );
    /* remaining is unknown */

    MP4_READBOX_EXIT( 1 );
}