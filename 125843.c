int MP4_ReadBoxContainerChildren( stream_t *p_stream, MP4_Box_t *p_container,
                                  uint32_t i_last_child )
{
    return MP4_ReadBoxContainerChildrenIndexed( p_stream, p_container,
                                                i_last_child, false );
}