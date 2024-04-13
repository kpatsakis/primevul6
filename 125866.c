static int MP4_NextBox( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_t box;

    if( !p_box )
    {
        if ( !MP4_ReadBoxCommon( p_stream, &box ) )
            return 0;
        p_box = &box;
    }

    if( !p_box->i_size )
    {
        return 2; /* Box with infinite size */
    }

    if( p_box->p_father )
    {
        /* if father's size == 0, it means unknown or infinite size,
         * and we skip the followong check */
        if( p_box->p_father->i_size > 0 )
        {
            const off_t i_box_end = p_box->i_size + p_box->i_pos;
            const off_t i_father_end = p_box->p_father->i_size + p_box->p_father->i_pos;

            /* check if it's within p-father */
            if( i_box_end >= i_father_end )
            {
                if( i_box_end > i_father_end )
                    msg_Dbg( p_stream, "out of bound child" );
                return 0; /* out of bound */
            }
        }
    }
    if( stream_Seek( p_stream, p_box->i_size + p_box->i_pos ) )
    {
        return 0;
    }

    return 1;
}