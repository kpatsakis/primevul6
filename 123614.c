static SchroFrame *CreateSchroFrameFromPic( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    SchroFrame *p_schroframe = schro_frame_new();
    picture_t *p_pic = NULL;
    struct picture_free_t *p_free;

    if( !p_schroframe )
        return NULL;

    p_pic = decoder_NewPicture( p_dec );

    if( !p_pic )
        return NULL;

    p_schroframe->format = SCHRO_FRAME_FORMAT_U8_420;
    if( p_sys->p_format->chroma_format == SCHRO_CHROMA_422 )
    {
        p_schroframe->format = SCHRO_FRAME_FORMAT_U8_422;
    }
    else if( p_sys->p_format->chroma_format == SCHRO_CHROMA_444 )
    {
        p_schroframe->format = SCHRO_FRAME_FORMAT_U8_444;
    }

    p_schroframe->width = p_sys->p_format->width;
    p_schroframe->height = p_sys->p_format->height;

    p_free = malloc( sizeof( *p_free ) );
    p_free->p_pic = p_pic;
    p_free->p_dec = p_dec;
    schro_frame_set_free_callback( p_schroframe, SchroFrameFree, p_free );

    for( int i=0; i<3; i++ )
    {
        p_schroframe->components[i].width = p_pic->p[i].i_visible_pitch;
        p_schroframe->components[i].stride = p_pic->p[i].i_pitch;
        p_schroframe->components[i].height = p_pic->p[i].i_visible_lines;
        p_schroframe->components[i].length =
            p_pic->p[i].i_pitch * p_pic->p[i].i_lines;
        p_schroframe->components[i].data = p_pic->p[i].p_pixels;

        if(i!=0)
        {
            p_schroframe->components[i].v_shift =
                SCHRO_FRAME_FORMAT_V_SHIFT( p_schroframe->format );
            p_schroframe->components[i].h_shift =
                SCHRO_FRAME_FORMAT_H_SHIFT( p_schroframe->format );
        }
    }

    p_pic->b_progressive = !p_sys->p_format->interlaced;
    p_pic->b_top_field_first = p_sys->p_format->top_field_first;
    p_pic->i_nb_fields = 2;

    return p_schroframe;
}