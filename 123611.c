static int ReadDiracPictureNumber( uint32_t *p_picnum, block_t *p_block )
{
    uint32_t u_pos = 4;
    /* protect against falling off the edge */
    while( u_pos + 13 < p_block->i_buffer )
    {
        /* find the picture startcode */
        if( p_block->p_buffer[u_pos] & 0x08 )
        {
            *p_picnum = GetDWBE( p_block->p_buffer + u_pos + 9 );
            return 1;
        }
        /* skip to the next dirac data unit */
        uint32_t u_npo = GetDWBE( p_block->p_buffer + u_pos + 1 );
        assert( u_npo <= UINT32_MAX - u_pos );
        if( u_npo == 0 )
            u_npo = 13;
        u_pos += u_npo;
    }
    return 0;
}