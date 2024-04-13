static int MP4_ReadBox_strf( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_strf_t );

    MP4_Box_data_strf_t *p_strf = p_box->data.p_strf;

    MP4_GET4BYTESLE( p_strf->bmiHeader.biSize );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biWidth );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biHeight );
    MP4_GET2BYTESLE( p_strf->bmiHeader.biPlanes );
    MP4_GET2BYTESLE( p_strf->bmiHeader.biBitCount );
    MP4_GETFOURCC( p_strf->bmiHeader.biCompression );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biSizeImage );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biXPelsPerMeter );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biYPelsPerMeter );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biClrUsed );
    MP4_GET4BYTESLE( p_strf->bmiHeader.biClrImportant );

    if ( i_read < 0 )
        goto error;

    p_strf->i_extra = i_read;
    if ( p_strf->i_extra )
    {
        p_strf->p_extra = malloc( p_strf->i_extra );
        if ( ! p_strf->p_extra )
            goto error;
        memcpy( p_strf->p_extra, p_peek, i_read );
    }

    MP4_READBOX_EXIT( 1 );

error:
    MP4_READBOX_EXIT( 0 );
}