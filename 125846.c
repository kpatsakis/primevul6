static int MP4_ReadBox_stdp( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_stdp_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_stdp );

    p_box->data.p_stdp->i_priority =
        calloc( i_read / 2, sizeof(uint16_t) );

    if( unlikely( !p_box->data.p_stdp->i_priority ) )
        MP4_READBOX_EXIT( 0 );

    for( unsigned i = 0; i < i_read / 2 ; i++ )
    {
        MP4_GET2BYTES( p_box->data.p_stdp->i_priority[i] );
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"stdp\" entry-count %"PRId64,
                      i_read / 2 );

#endif
    MP4_READBOX_EXIT( 1 );
}