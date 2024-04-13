MP4_Box_t *MP4_BoxGetNextChunk( stream_t *s )
{
    /* p_chunk is a virtual root container for the moof and mdat boxes */
    MP4_Box_t *p_chunk;
    MP4_Box_t *p_tmp_box = NULL;

    p_tmp_box = calloc( 1, sizeof( MP4_Box_t ) );
    if( unlikely( p_tmp_box == NULL ) )
        return NULL;

    /* We might get a ftyp box or a SmooBox */
    MP4_ReadBoxCommon( s, p_tmp_box );

    if( (p_tmp_box->i_type == ATOM_uuid && !CmpUUID( &p_tmp_box->i_uuid, &SmooBoxUUID )) )
    {
        free( p_tmp_box );
        return MP4_BoxGetSmooBox( s );
    }
    else if( p_tmp_box->i_type == ATOM_ftyp )
    {
        free( p_tmp_box );
        return MP4_BoxGetRoot( s );
    }
    free( p_tmp_box );

    p_chunk = calloc( 1, sizeof( MP4_Box_t ) );
    if( unlikely( p_chunk == NULL ) )
        return NULL;

    p_chunk->i_type = ATOM_root;
    p_chunk->i_shortsize = 1;

    MP4_ReadBoxContainerChildren( s, p_chunk, ATOM_moof );

    p_tmp_box = p_chunk->p_first;
    while( p_tmp_box )
    {
        p_chunk->i_size += p_tmp_box->i_size;
        p_tmp_box = p_tmp_box->p_next;
    }

    return p_chunk;
}