static int MP4_ReadBoxContainerChildrenIndexed( stream_t *p_stream,
               MP4_Box_t *p_container, uint32_t i_last_child, bool b_indexed )
{
    MP4_Box_t *p_box;

    /* Size of root container is set to 0 when unknown, for exemple
     * with a DASH stream. In that case, we skip the following check */
    if( p_container->i_size
            && ( stream_Tell( p_stream ) + ((b_indexed)?16:8) >
        (off_t)(p_container->i_pos + p_container->i_size) )
      )
    {
        /* there is no box to load */
        return 0;
    }

    do
    {
        uint32_t i_index = 0;
        if ( b_indexed )
        {
            uint8_t read[8];
            if ( stream_Read( p_stream, read, 8 ) < 8 )
                return 0;
            i_index = GetDWBE(&read[4]);
        }
        if( ( p_box = MP4_ReadBox( p_stream, p_container ) ) == NULL ) continue;
        p_box->i_index = i_index;

        /* chain this box with the father and the other at same level */
        if( !p_container->p_first ) p_container->p_first = p_box;
        else p_container->p_last->p_next = p_box;
        p_container->p_last = p_box;

        if( p_box->i_type == i_last_child )
        {
            MP4_NextBox( p_stream, p_box );
            break;
        }

    } while( MP4_NextBox( p_stream, p_box ) == 1 );

    return 1;
}