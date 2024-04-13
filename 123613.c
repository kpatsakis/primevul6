static void CloseEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;

    /* Free the encoder resources */
    if( p_sys->p_schro )
        schro_encoder_free( p_sys->p_schro );

    free( p_sys->p_format );

    if( p_sys->p_dts_fifo )
        block_FifoRelease( p_sys->p_dts_fifo );

    block_ChainRelease( p_sys->p_chain );

    free( p_sys );
}