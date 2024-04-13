static int MP4_ReadBox_esds( stream_t *p_stream, MP4_Box_t *p_box )
{
#define es_descriptor p_box->data.p_esds->es_descriptor
    unsigned int i_len;
    unsigned int i_flags;
    unsigned int i_type;

    MP4_READBOX_ENTER( MP4_Box_data_esds_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_esds );


    MP4_GET1BYTE( i_type );
    if( i_type == 0x03 ) /* MP4ESDescrTag ISO/IEC 14496-1 8.3.3 */
    {
        i_len = MP4_ReadLengthDescriptor( &p_peek, &i_read );

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "found esds MPEG4ESDescr (%dBytes)",
                 i_len );
#endif

        MP4_GET2BYTES( es_descriptor.i_ES_ID );
        MP4_GET1BYTE( i_flags );
        es_descriptor.b_stream_dependence = ( (i_flags&0x80) != 0);
        es_descriptor.b_url = ( (i_flags&0x40) != 0);
        es_descriptor.b_OCRstream = ( (i_flags&0x20) != 0);

        es_descriptor.i_stream_priority = i_flags&0x1f;
        if( es_descriptor.b_stream_dependence )
        {
            MP4_GET2BYTES( es_descriptor.i_depend_on_ES_ID );
        }
        if( es_descriptor.b_url )
        {
            unsigned int i_len;

            MP4_GET1BYTE( i_len );
            i_len = __MIN(i_read, i_len);
            es_descriptor.psz_URL = malloc( i_len + 1 );
            if( es_descriptor.psz_URL )
            {
                memcpy( es_descriptor.psz_URL, p_peek, i_len );
                es_descriptor.psz_URL[i_len] = 0;
            }
            p_peek += i_len;
            i_read -= i_len;
        }
        else
        {
            es_descriptor.psz_URL = NULL;
        }
        if( es_descriptor.b_OCRstream )
        {
            MP4_GET2BYTES( es_descriptor.i_OCR_ES_ID );
        }
        MP4_GET1BYTE( i_type ); /* get next type */
    }

    if( i_type != 0x04)/* MP4DecConfigDescrTag ISO/IEC 14496-1 8.3.4 */
    {
         es_descriptor.p_decConfigDescr = NULL;
         MP4_READBOX_EXIT( 1 ); /* rest isn't interesting up to now */
    }

    i_len = MP4_ReadLengthDescriptor( &p_peek, &i_read );

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "found esds MP4DecConfigDescr (%dBytes)",
                 i_len );
#endif

    es_descriptor.p_decConfigDescr =
            calloc( 1, sizeof( MP4_descriptor_decoder_config_t ));
    if( unlikely( es_descriptor.p_decConfigDescr == NULL ) )
        MP4_READBOX_EXIT( 0 );

    MP4_GET1BYTE( es_descriptor.p_decConfigDescr->i_objectProfileIndication );
    MP4_GET1BYTE( i_flags );
    es_descriptor.p_decConfigDescr->i_streamType = i_flags >> 2;
    es_descriptor.p_decConfigDescr->b_upStream = ( i_flags >> 1 )&0x01;
    MP4_GET3BYTES( es_descriptor.p_decConfigDescr->i_buffer_sizeDB );
    MP4_GET4BYTES( es_descriptor.p_decConfigDescr->i_max_bitrate );
    MP4_GET4BYTES( es_descriptor.p_decConfigDescr->i_avg_bitrate );
    MP4_GET1BYTE( i_type );
    if( i_type !=  0x05 )/* MP4DecSpecificDescrTag ISO/IEC 14496-1 8.3.5 */
    {
        es_descriptor.p_decConfigDescr->i_decoder_specific_info_len = 0;
        es_descriptor.p_decConfigDescr->p_decoder_specific_info  = NULL;
        MP4_READBOX_EXIT( 1 );
    }

    i_len = MP4_ReadLengthDescriptor( &p_peek, &i_read );

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "found esds MP4DecSpecificDescr (%dBytes)",
                 i_len );
#endif
    if( i_len > i_read )
        MP4_READBOX_EXIT( 0 );

    es_descriptor.p_decConfigDescr->i_decoder_specific_info_len = i_len;
    es_descriptor.p_decConfigDescr->p_decoder_specific_info = malloc( i_len );
    if( unlikely( es_descriptor.p_decConfigDescr->p_decoder_specific_info == NULL ) )
        MP4_READBOX_EXIT( 0 );

    memcpy( es_descriptor.p_decConfigDescr->p_decoder_specific_info,
            p_peek, i_len );

    MP4_READBOX_EXIT( 1 );
#undef es_descriptor
}