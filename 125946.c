static MP4_Box_t *MP4_ReadBox( stream_t *p_stream, MP4_Box_t *p_father )
{
    MP4_Box_t *p_box = calloc( 1, sizeof( MP4_Box_t ) ); /* Needed to ensure simple on error handler */
    unsigned int i_index;

    if( p_box == NULL )
        return NULL;

    if( !MP4_ReadBoxCommon( p_stream, p_box ) )
    {
        msg_Warn( p_stream, "cannot read one box" );
        free( p_box );
        return NULL;
    }
    if( !p_box->i_size )
    {
        msg_Dbg( p_stream, "found an empty box (null size)" );
        free( p_box );
        return NULL;
    }
    p_box->p_father = p_father;

    /* Now search function to call */
    for( i_index = 0; ; i_index++ )
    {
        if ( MP4_Box_Function[i_index].i_parent &&
             p_box->p_father &&
             p_box->p_father->i_type != MP4_Box_Function[i_index].i_parent )
            continue;

        if( ( MP4_Box_Function[i_index].i_type == p_box->i_type )||
            ( MP4_Box_Function[i_index].i_type == 0 ) )
        {
            break;
        }
    }

    if( !(MP4_Box_Function[i_index].MP4_ReadBox_function)( p_stream, p_box ) )
    {
        off_t i_end = p_box->i_pos + p_box->i_size;
        MP4_BoxFree( p_stream, p_box );
        stream_Seek( p_stream, i_end ); /* Skip the failed box */
        return NULL;
    }

    p_box->pf_free = MP4_Box_Function[i_index].MP4_FreeBox_function;

    return p_box;
}