static int MP4_ReadBox_dvc1( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_data_dvc1_t *p_dvc1;

    MP4_READBOX_ENTER( MP4_Box_data_dvc1_t );
    p_dvc1 = p_box->data.p_dvc1;

    MP4_GET1BYTE( p_dvc1->i_profile_level ); /* profile is on 4bits, level 3bits */
    uint8_t i_profile = (p_dvc1->i_profile_level & 0xf0) >> 4;
    if( i_profile != 0x06 && i_profile != 0x0c )
    {
        msg_Warn( p_stream, "unsupported VC-1 profile (%"PRIu8"), please report", i_profile );
        MP4_READBOX_EXIT( 0 );
    }


    p_dvc1->i_vc1 = p_box->i_size - 7; /* Header + profile_level */

    if( p_dvc1->i_vc1 > 0 )
    {
        uint8_t *p = p_dvc1->p_vc1 = malloc( p_dvc1->i_vc1 );
        if( p )
            memcpy( p, p_peek, i_read );
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"dvc1\" profile=%"PRIu8" level=%i",
             i_profile, p_dvc1->i_profile_level & 0x0e >> 1 );
#endif

    MP4_READBOX_EXIT( 1 );
}