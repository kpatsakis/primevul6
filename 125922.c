static int MP4_ReadBox_default( stream_t *p_stream, MP4_Box_t *p_box )
{
    if( !p_box->p_father )
    {
        goto unknown;
    }
    if( p_box->p_father->i_type == ATOM_stsd )
    {
        MP4_Box_t *p_mdia = MP4_BoxGet( p_box, "../../../.." );
        MP4_Box_t *p_hdlr;

        if( p_mdia == NULL || p_mdia->i_type != ATOM_mdia ||
            (p_hdlr = MP4_BoxGet( p_mdia, "hdlr" )) == NULL )
        {
            goto unknown;
        }
        switch( p_hdlr->data.p_hdlr->i_handler_type )
        {
            case ATOM_soun:
                return MP4_ReadBox_sample_soun( p_stream, p_box );
            case ATOM_vide:
                return MP4_ReadBox_sample_vide( p_stream, p_box );
            case ATOM_text:
                return MP4_ReadBox_sample_text( p_stream, p_box );
            case ATOM_tx3g:
            case ATOM_sbtl:
                return MP4_ReadBox_sample_tx3g( p_stream, p_box );
            default:
                msg_Warn( p_stream,
                          "unknown handler type in stsd (incompletely loaded)" );
                return 1;
        }
    }

unknown:
    if MP4_BOX_TYPE_ASCII()
        msg_Warn( p_stream,
                "unknown box type %4.4s (incompletely loaded)",
                (char*)&p_box->i_type );
    else
        msg_Warn( p_stream,
                "unknown box type c%3.3s (incompletely loaded)",
                (char*)&p_box->i_type+1 );
    p_box->e_flags |= BOX_FLAG_INCOMPLETE;

    return 1;
}