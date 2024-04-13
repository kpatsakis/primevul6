static int MP4_ReadBox_avcC( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_data_avcC_t *p_avcC;
    int i;

    MP4_READBOX_ENTER( MP4_Box_data_avcC_t );
    p_avcC = p_box->data.p_avcC;

    p_avcC->i_avcC = i_read;
    if( p_avcC->i_avcC > 0 )
    {
        uint8_t * p = p_avcC->p_avcC = malloc( p_avcC->i_avcC );
        if( p )
            memcpy( p, p_peek, i_read );
    }

    MP4_GET1BYTE( p_avcC->i_version );
    MP4_GET1BYTE( p_avcC->i_profile );
    MP4_GET1BYTE( p_avcC->i_profile_compatibility );
    MP4_GET1BYTE( p_avcC->i_level );
    MP4_GET1BYTE( p_avcC->i_reserved1 );
    p_avcC->i_length_size = (p_avcC->i_reserved1&0x03) + 1;
    p_avcC->i_reserved1 >>= 2;

    MP4_GET1BYTE( p_avcC->i_reserved2 );
    p_avcC->i_sps = p_avcC->i_reserved2&0x1f;
    p_avcC->i_reserved2 >>= 5;

    if( p_avcC->i_sps > 0 )
    {
        p_avcC->i_sps_length = calloc( p_avcC->i_sps, sizeof( uint16_t ) );
        p_avcC->sps = calloc( p_avcC->i_sps, sizeof( uint8_t* ) );

        if( !p_avcC->i_sps_length || !p_avcC->sps )
            goto error;

        for( i = 0; i < p_avcC->i_sps && i_read > 2; i++ )
        {
            MP4_GET2BYTES( p_avcC->i_sps_length[i] );
            if ( p_avcC->i_sps_length[i] > i_read )
                goto error;
            p_avcC->sps[i] = malloc( p_avcC->i_sps_length[i] );
            if( p_avcC->sps[i] )
                memcpy( p_avcC->sps[i], p_peek, p_avcC->i_sps_length[i] );

            p_peek += p_avcC->i_sps_length[i];
            i_read -= p_avcC->i_sps_length[i];
        }
        if ( i != p_avcC->i_sps )
            goto error;
    }

    MP4_GET1BYTE( p_avcC->i_pps );
    if( p_avcC->i_pps > 0 )
    {
        p_avcC->i_pps_length = calloc( p_avcC->i_pps, sizeof( uint16_t ) );
        p_avcC->pps = calloc( p_avcC->i_pps, sizeof( uint8_t* ) );

        if( !p_avcC->i_pps_length || !p_avcC->pps )
            goto error;

        for( i = 0; i < p_avcC->i_pps && i_read > 2; i++ )
        {
            MP4_GET2BYTES( p_avcC->i_pps_length[i] );
            if( p_avcC->i_pps_length[i] > i_read )
                goto error;
            p_avcC->pps[i] = malloc( p_avcC->i_pps_length[i] );
            if( p_avcC->pps[i] )
                memcpy( p_avcC->pps[i], p_peek, p_avcC->i_pps_length[i] );

            p_peek += p_avcC->i_pps_length[i];
            i_read -= p_avcC->i_pps_length[i];
        }
        if ( i != p_avcC->i_pps )
            goto error;
    }
#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"avcC\" version=%d profile=0x%x level=0x%x length size=%d sps=%d pps=%d",
             p_avcC->i_version, p_avcC->i_profile, p_avcC->i_level,
             p_avcC->i_length_size,
             p_avcC->i_sps, p_avcC->i_pps );
    for( i = 0; i < p_avcC->i_sps; i++ )
    {
        msg_Dbg( p_stream, "         - sps[%d] length=%d",
                 i, p_avcC->i_sps_length[i] );
    }
    for( i = 0; i < p_avcC->i_pps; i++ )
    {
        msg_Dbg( p_stream, "         - pps[%d] length=%d",
                 i, p_avcC->i_pps_length[i] );
    }

#endif
    MP4_READBOX_EXIT( 1 );

error:
    MP4_FreeBox_avcC( p_box );
    MP4_READBOX_EXIT( 0 );
}