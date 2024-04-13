static int MP4_ReadBox_WMA2( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_WMA2_t );

    MP4_Box_data_WMA2_t *p_WMA2 = p_box->data.p_WMA2;

    MP4_GET2BYTESLE( p_WMA2->Format.wFormatTag );
    MP4_GET2BYTESLE( p_WMA2->Format.nChannels );
    MP4_GET4BYTESLE( p_WMA2->Format.nSamplesPerSec );
    MP4_GET4BYTESLE( p_WMA2->Format.nAvgBytesPerSec );
    MP4_GET2BYTESLE( p_WMA2->Format.nBlockAlign );
    MP4_GET2BYTESLE( p_WMA2->Format.wBitsPerSample );

    uint16_t i_cbSize;
    MP4_GET2BYTESLE( i_cbSize );

    if ( i_read < 0 || i_cbSize > i_read )
        goto error;

    p_WMA2->i_extra = i_cbSize;
    if ( p_WMA2->i_extra )
    {
        p_WMA2->p_extra = malloc( p_WMA2->i_extra );
        if ( ! p_WMA2->p_extra )
            goto error;
        memcpy( p_WMA2->p_extra, p_peek, p_WMA2->i_extra );
    }

    MP4_READBOX_EXIT( 1 );

error:
    MP4_READBOX_EXIT( 0 );
}