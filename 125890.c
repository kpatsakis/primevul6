int MP4_BoxCount( MP4_Box_t *p_box, const char *psz_fmt, ... )
{
    va_list args;
    int     i_count;
    MP4_Box_t *p_result, *p_next;

    va_start( args, psz_fmt );
    MP4_BoxGet_Internal( &p_result, p_box, psz_fmt, args );
    va_end( args );
    if( !p_result )
    {
        return( 0 );
    }

    i_count = 1;
    for( p_next = p_result->p_next; p_next != NULL; p_next = p_next->p_next)
    {
        if( p_next->i_type == p_result->i_type)
        {
            i_count++;
        }
    }
    return( i_count );
}