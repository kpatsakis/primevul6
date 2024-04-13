MP4_Box_t *MP4_BoxGetSmooBox( stream_t *s )
{
    /* p_chunk is a virtual root container for the smoo box */
    MP4_Box_t *p_chunk;
    MP4_Box_t *p_smoo;

    p_chunk = calloc( 1, sizeof( MP4_Box_t ) );
    if( unlikely( p_chunk == NULL ) )
        return NULL;

    p_chunk->i_type = ATOM_root;
    p_chunk->i_shortsize = 1;

    p_smoo = MP4_ReadBox( s, p_chunk );
    if( !p_smoo || p_smoo->i_type != ATOM_uuid || CmpUUID( &p_smoo->i_uuid, &SmooBoxUUID ) )
    {
        msg_Warn( s, "no smoo box found!");
        goto error;
    }

    p_chunk->p_first = p_smoo;
    p_chunk->p_last = p_smoo;

    return p_chunk;

error:
    free( p_chunk );
    return NULL;
}