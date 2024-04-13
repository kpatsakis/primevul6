static int MP4_ReadBox_stsdext_chan( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_chan_t );
    MP4_Box_data_chan_t *p_chan = p_box->data.p_chan;

    if ( i_read < 16 )
        MP4_READBOX_EXIT( 0 );

    MP4_GET1BYTE( p_chan->i_version );
    MP4_GET3BYTES( p_chan->i_channels_flags );
    MP4_GET4BYTES( p_chan->layout.i_channels_layout_tag );
    MP4_GET4BYTES( p_chan->layout.i_channels_bitmap );
    MP4_GET4BYTES( p_chan->layout.i_channels_description_count );

    size_t i_descsize = 8 + 3 * sizeof(float);
    if ( (size_t)i_read < p_chan->layout.i_channels_description_count * i_descsize )
        MP4_READBOX_EXIT( 0 );

    p_chan->layout.p_descriptions =
        malloc( p_chan->layout.i_channels_description_count * i_descsize );

    if ( !p_chan->layout.p_descriptions )
        MP4_READBOX_EXIT( 0 );

    uint32_t i;
    for( i=0; i<p_chan->layout.i_channels_description_count; i++ )
    {
        if ( i_read < 20 )
            break;
        MP4_GET4BYTES( p_chan->layout.p_descriptions[i].i_channel_label );
        MP4_GET4BYTES( p_chan->layout.p_descriptions[i].i_channel_flags );
        MP4_GET4BYTES( p_chan->layout.p_descriptions[i].f_coordinates[0] );
        MP4_GET4BYTES( p_chan->layout.p_descriptions[i].f_coordinates[1] );
        MP4_GET4BYTES( p_chan->layout.p_descriptions[i].f_coordinates[2] );
    }
    if ( i<p_chan->layout.i_channels_description_count )
        p_chan->layout.i_channels_description_count = i;

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"chan\" flags=0x%x tag=0x%x bitmap=0x%x descriptions=%u",
             p_chan->i_channels_flags, p_chan->layout.i_channels_layout_tag,
             p_chan->layout.i_channels_bitmap, p_chan->layout.i_channels_description_count );
#endif
    MP4_READBOX_EXIT( 1 );
}