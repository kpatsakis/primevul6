static int MP4_ReadBox_Binary( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_binary_t );
    i_read = __MIN( i_read, UINT32_MAX );
    if ( i_read > 0 )
    {
        p_box->data.p_binary->p_blob = malloc( i_read );
        if ( p_box->data.p_binary->p_blob )
        {
            memcpy( p_box->data.p_binary->p_blob, p_peek, i_read );
            p_box->data.p_binary->i_blob = i_read;
        }
    }
    MP4_READBOX_EXIT( 1 );
}