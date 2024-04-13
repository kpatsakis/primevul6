MP4_Box_t *MP4_BoxGetRoot( stream_t *s )
{
    MP4_Box_t *p_root;
    stream_t *p_stream;
    int i_result;

    p_root = malloc( sizeof( MP4_Box_t ) );
    if( p_root == NULL )
        return NULL;

    p_root->i_pos = 0;
    p_root->i_type = ATOM_root;
    p_root->i_shortsize = 1;
    /* could be a DASH stream for exemple, 0 means unknown or infinite size */
    p_root->i_size = 0;
    CreateUUID( &p_root->i_uuid, p_root->i_type );

    p_root->data.p_payload = NULL;
    p_root->p_father    = NULL;
    p_root->p_first     = NULL;
    p_root->p_last      = NULL;
    p_root->p_next      = NULL;

    p_stream = s;

    /* First get the moov */
    i_result = MP4_ReadBoxContainerChildren( p_stream, p_root, ATOM_moov );

    if( !i_result )
        goto error;
    /* If there is a mvex box, it means fragmented MP4, and we're done */
    else if( MP4_BoxCount( p_root, "moov/mvex" ) > 0 )
        return p_root;

    p_root->i_size = stream_Size( s );
    if( stream_Tell( s ) + 8 < stream_Size( s ) )
    {
        /* Get the rest of the file */
        i_result = MP4_ReadBoxContainerRaw( p_stream, p_root );

        if( !i_result )
            goto error;
    }

    MP4_Box_t *p_moov;
    MP4_Box_t *p_cmov;

    /* check if there is a cmov, if so replace
      compressed moov by  uncompressed one */
    if( ( ( p_moov = MP4_BoxGet( p_root, "moov" ) ) &&
          ( p_cmov = MP4_BoxGet( p_root, "moov/cmov" ) ) ) ||
        ( ( p_moov = MP4_BoxGet( p_root, "foov" ) ) &&
          ( p_cmov = MP4_BoxGet( p_root, "foov/cmov" ) ) ) )
    {
        /* rename the compressed moov as a box to skip */
        p_moov->i_type = ATOM_skip;

        /* get uncompressed p_moov */
        p_moov = p_cmov->data.p_cmov->p_moov;
        p_cmov->data.p_cmov->p_moov = NULL;

        /* make p_root father of this new moov */
        p_moov->p_father = p_root;

        /* insert this new moov box as first child of p_root */
        p_moov->p_next = p_root->p_first;
        p_root->p_first = p_moov;
    }

    return p_root;

error:
    free( p_root );
    stream_Seek( p_stream, 0 );
    return NULL;
}