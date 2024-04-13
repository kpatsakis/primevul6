static int MP4_ReadBoxContainer( stream_t *p_stream, MP4_Box_t *p_container )
{
    if( p_container->i_size &&
        ( p_container->i_size <= (size_t)mp4_box_headersize(p_container ) + 8 ) )
    {
        /* container is empty, 8 stand for the first header in this box */
        return 1;
    }

    /* enter box */
    stream_Seek( p_stream, p_container->i_pos +
                 mp4_box_headersize( p_container ) );

    return MP4_ReadBoxContainerRaw( p_stream, p_container );
}