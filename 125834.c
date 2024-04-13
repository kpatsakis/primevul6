static int MP4_ReadBox_hvcC( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_data_hvcC_t *p_hvcC;

    MP4_READBOX_ENTER( MP4_Box_data_hvcC_t );
    p_hvcC = p_box->data.p_hvcC;

    p_hvcC->i_hvcC = i_read;
    if( p_hvcC->i_hvcC > 0 )
    {
        uint8_t * p = p_hvcC->p_hvcC = malloc( p_hvcC->i_hvcC );
        if( p )
            memcpy( p, p_peek, i_read );
    }
    MP4_READBOX_EXIT( 1 );
}