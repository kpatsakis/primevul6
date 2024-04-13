static void MP4_FreeBox_padb( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_padb->i_reserved1 );
    FREENULL( p_box->data.p_padb->i_pad2 );
    FREENULL( p_box->data.p_padb->i_reserved2 );
    FREENULL( p_box->data.p_padb->i_pad1 );
}