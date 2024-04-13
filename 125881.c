static int MP4_ReadBox_chpl( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_data_chpl_t *p_chpl;
    uint32_t i_dummy;
    VLC_UNUSED(i_dummy);
    int i;
    MP4_READBOX_ENTER( MP4_Box_data_chpl_t );

    p_chpl = p_box->data.p_chpl;

    MP4_GETVERSIONFLAGS( p_chpl );

    if ( i_read < 5 || p_chpl->i_version != 0x1 )
        MP4_READBOX_EXIT( 0 );

    MP4_GET4BYTES( i_dummy );

    MP4_GET1BYTE( p_chpl->i_chapter );

    for( i = 0; i < p_chpl->i_chapter; i++ )
    {
        uint64_t i_start;
        uint8_t i_len;
        int i_copy;
        if ( i_read < 9 )
            break;
        MP4_GET8BYTES( i_start );
        MP4_GET1BYTE( i_len );

        p_chpl->chapter[i].psz_name = malloc( i_len + 1 );
        if( !p_chpl->chapter[i].psz_name )
            MP4_READBOX_EXIT( 0 );

        i_copy = __MIN( i_len, i_read );
        if( i_copy > 0 )
            memcpy( p_chpl->chapter[i].psz_name, p_peek, i_copy );
        p_chpl->chapter[i].psz_name[i_copy] = '\0';
        p_chpl->chapter[i].i_start = i_start;

        p_peek += i_copy;
        i_read -= i_copy;
    }

    if ( i != p_chpl->i_chapter )
        p_chpl->i_chapter = i;

    /* Bubble sort by increasing start date */
    do
    {
        for( i = 0; i < p_chpl->i_chapter - 1; i++ )
        {
            if( p_chpl->chapter[i].i_start > p_chpl->chapter[i+1].i_start )
            {
                char *psz = p_chpl->chapter[i+1].psz_name;
                int64_t i64 = p_chpl->chapter[i+1].i_start;

                p_chpl->chapter[i+1].psz_name = p_chpl->chapter[i].psz_name;
                p_chpl->chapter[i+1].i_start = p_chpl->chapter[i].i_start;

                p_chpl->chapter[i].psz_name = psz;
                p_chpl->chapter[i].i_start = i64;

                i = -1;
                break;
            }
        }
    } while( i == -1 );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"chpl\" %d chapters",
                       p_chpl->i_chapter );
#endif
    MP4_READBOX_EXIT( 1 );
}