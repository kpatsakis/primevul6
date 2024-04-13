MP4_Box_t *MP4_BoxGet( MP4_Box_t *p_box, const char *psz_fmt, ... )
{
    va_list args;
    MP4_Box_t *p_result;

    va_start( args, psz_fmt );
    MP4_BoxGet_Internal( &p_result, p_box, psz_fmt, args );
    va_end( args );

    return( p_result );
}