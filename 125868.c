void MP4_BoxFree( stream_t *s, MP4_Box_t *p_box )
{
    MP4_Box_t    *p_child;

    if( !p_box )
        return; /* hehe */

    for( p_child = p_box->p_first; p_child != NULL; )
    {
        MP4_Box_t *p_next;

        p_next = p_child->p_next;
        MP4_BoxFree( s, p_child );
        p_child = p_next;
    }

    /* Now search function to call */
    if( p_box->data.p_payload )
    {
        if (unlikely( p_box->pf_free == NULL ))
        {
            /* Should not happen */
            if MP4_BOX_TYPE_ASCII()
                msg_Warn( s,
                        "cannot free box %4.4s, type unknown",
                        (char*)&p_box->i_type );
            else
                msg_Warn( s,
                        "cannot free box c%3.3s, type unknown",
                        (char*)&p_box->i_type+1 );
        }
        else
        {
            p_box->pf_free( p_box );
        }
        free( p_box->data.p_payload );
    }
    free( p_box );
}