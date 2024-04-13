static int MP4_ReadBox_stra( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_stra_t );
    MP4_Box_data_stra_t *p_stra = p_box->data.p_stra;

    uint8_t i_reserved;
    VLC_UNUSED(i_reserved);
    MP4_GET1BYTE( p_stra->i_es_cat );
    MP4_GET1BYTE( i_reserved );
    MP4_GET2BYTES( p_stra->i_track_ID );

    MP4_GET4BYTES( p_stra->i_timescale );
    MP4_GET8BYTES( p_stra->i_duration );

    MP4_GET4BYTES( p_stra->FourCC );
    MP4_GET4BYTES( p_stra->Bitrate );
    MP4_GET4BYTES( p_stra->MaxWidth );
    MP4_GET4BYTES( p_stra->MaxHeight );
    MP4_GET4BYTES( p_stra->SamplingRate );
    MP4_GET4BYTES( p_stra->Channels );
    MP4_GET4BYTES( p_stra->BitsPerSample );
    MP4_GET4BYTES( p_stra->AudioTag );
    MP4_GET2BYTES( p_stra->nBlockAlign );

    MP4_GET1BYTE( i_reserved );
    MP4_GET1BYTE( i_reserved );
    MP4_GET1BYTE( i_reserved );
    MP4_GET1BYTE( p_stra->cpd_len );
    if( p_stra->cpd_len > i_read )
        goto error;
    p_stra->CodecPrivateData = malloc( p_stra->cpd_len );
    if( unlikely( p_stra->CodecPrivateData == NULL ) )
        goto error;
    memcpy( p_stra->CodecPrivateData, p_peek, p_stra->cpd_len );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "es_cat is %"PRIu8", birate is %"PRIu32,
              p_stra->i_es_cat, p_stra->Bitrate );
#endif

    MP4_READBOX_EXIT( 1 );
error:
    MP4_READBOX_EXIT( 0 );
}