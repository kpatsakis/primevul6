static int MP4_ReadBox_keys( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_keys_t );

    if ( i_read < 8 )
        MP4_READBOX_EXIT( 0 );

    uint32_t i_count;
    MP4_GET4BYTES( i_count ); /* reserved + flags */
    if ( i_count != 0 )
        MP4_READBOX_EXIT( 0 );

    MP4_GET4BYTES( i_count );
    p_box->data.p_keys->p_entries = calloc( i_count, sizeof(*p_box->data.p_keys->p_entries) );
    if ( !p_box->data.p_keys->p_entries )
        MP4_READBOX_EXIT( 0 );
    p_box->data.p_keys->i_entry_count = i_count;

    uint32_t i=0;
    for( ; i < i_count; i++ )
    {
        if ( i_read < 8 )
            break;
        uint32_t i_keysize;
        MP4_GET4BYTES( i_keysize );
        if ( (i_keysize < 8) || (i_keysize - 4 > i_read) )
            break;
        MP4_GETFOURCC( p_box->data.p_keys->p_entries[i].i_namespace );
        i_keysize -= 8;
        p_box->data.p_keys->p_entries[i].psz_value = malloc( i_keysize + 1 );
        if ( !p_box->data.p_keys->p_entries[i].psz_value )
            break;
        memcpy( p_box->data.p_keys->p_entries[i].psz_value, p_peek, i_keysize );
        p_box->data.p_keys->p_entries[i].psz_value[i_keysize] = 0;
        p_peek += i_keysize;
        i_read -= i_keysize;
#ifdef MP4_ULTRA_VERBOSE
        msg_Dbg( p_stream, "read box: \"keys\": %u '%s'", i + 1,
                 p_box->data.p_keys->p_entries[i].psz_value );
#endif
    }
    if ( i < i_count )
        p_box->data.p_keys->i_entry_count = i;

    MP4_READBOX_EXIT( 1 );
}