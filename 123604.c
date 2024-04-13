static void EncSchroFrameFree( SchroFrame *frame, void *priv )
{
    struct enc_picture_free_t *p_free = priv;

    if( !p_free )
        return;

    picture_Release( p_free->p_pic );
    free( p_free );
    (void)frame;
}