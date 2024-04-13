static int MP4_ReadBox_ilst( stream_t *p_stream, MP4_Box_t *p_box )
{
    if( p_box->i_size < 8 || stream_Read( p_stream, NULL, 8 ) < 8 )
        return 0;

    /* Find our handler */
    if ( !p_box->i_handler && p_box->p_father )
    {
        const MP4_Box_t *p_sibling = p_box->p_father->p_first;
        while( p_sibling )
        {
            if ( p_sibling->i_type == ATOM_hdlr && p_sibling->data.p_hdlr )
            {
                p_box->i_handler = p_sibling->data.p_hdlr->i_handler_type;
                break;
            }
            p_sibling = p_sibling->p_next;
        }
    }

    switch( p_box->i_handler )
    {
    case 0:
        msg_Warn( p_stream, "no handler for ilst atom" );
        return 0;
    case HANDLER_mdta:
        return MP4_ReadBoxContainerChildrenIndexed( p_stream, p_box, 0, true );
    case HANDLER_mdir:
        return MP4_ReadBoxContainerChildren( p_stream, p_box, 0 );
    default:
        msg_Warn( p_stream, "Unknown ilst handler type '%4.4s'", (char*)&p_box->i_handler );
        return 0;
    }
}