static int MP4_ReadBoxContainerRaw( stream_t *p_stream, MP4_Box_t *p_container )
{
    return MP4_ReadBoxContainerChildren( p_stream, p_container, 0 );
}