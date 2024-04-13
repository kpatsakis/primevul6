static int MP4_ReadBox_stsd( stream_t *p_stream, MP4_Box_t *p_box )
{

    MP4_READBOX_ENTER( MP4_Box_data_stsd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_stsd );

    MP4_GET4BYTES( p_box->data.p_stsd->i_entry_count );

    stream_Seek( p_stream, p_box->i_pos + mp4_box_headersize( p_box ) + 8 );

    MP4_ReadBoxContainerRaw( p_stream, p_box );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"stsd\" entry-count %d",
                      p_box->data.p_stsd->i_entry_count );

#endif
    MP4_READBOX_EXIT( 1 );
}