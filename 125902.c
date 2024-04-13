static void MP4_BoxDumpStructure_Internal( stream_t *s,
                                    MP4_Box_t *p_box, unsigned int i_level )
{
    MP4_Box_t *p_child;
    uint32_t i_displayedtype = p_box->i_type;
    if( ! MP4_BOX_TYPE_ASCII() ) ((char*)&i_displayedtype)[0] = 'c';

    if( !i_level )
    {
        msg_Dbg( s, "dumping root Box \"%4.4s\"",
                          (char*)&i_displayedtype );
    }
    else
    {
        char str[512];
        if( i_level >= (sizeof(str) - 1)/4 )
            return;

        memset( str, ' ', sizeof(str) );
        for( unsigned i = 0; i < i_level; i++ )
        {
            str[i*4] = '|';
        }

        snprintf( &str[i_level * 4], sizeof(str) - 4*i_level,
                  "+ %4.4s size %"PRIu64" offset %" PRIuMAX "%s",
                    (char*)&i_displayedtype, p_box->i_size,
                  (uintmax_t)p_box->i_pos,
                p_box->e_flags & BOX_FLAG_INCOMPLETE ? " (\?\?\?\?)" : "" );
        msg_Dbg( s, "%s", str );
    }
    p_child = p_box->p_first;
    while( p_child )
    {
        MP4_BoxDumpStructure_Internal( s, p_child, i_level + 1 );
        p_child = p_child->p_next;
    }
}