static int MP4_ReadBox_sample_mp4s( stream_t *p_stream, MP4_Box_t *p_box )
{
    stream_Seek( p_stream, p_box->i_pos + mp4_box_headersize( p_box ) + 8 );
    MP4_ReadBoxContainerRaw( p_stream, p_box );
    return 1;
}