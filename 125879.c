static int MP4_ReadBox_sample_tx3g( stream_t *p_stream, MP4_Box_t *p_box )
{
    p_box->i_handler = ATOM_text;
    MP4_READBOX_ENTER( MP4_Box_data_sample_text_t );

    MP4_GET4BYTES( p_box->data.p_sample_text->i_reserved1 );
    MP4_GET2BYTES( p_box->data.p_sample_text->i_reserved2 );

    MP4_GET2BYTES( p_box->data.p_sample_text->i_data_reference_index );

    MP4_GET4BYTES( p_box->data.p_sample_text->i_display_flags );

    MP4_GET1BYTE ( p_box->data.p_sample_text->i_justification_horizontal );
    MP4_GET1BYTE ( p_box->data.p_sample_text->i_justification_vertical );

    MP4_GET1BYTE ( p_box->data.p_sample_text->i_background_color[0] );
    MP4_GET1BYTE ( p_box->data.p_sample_text->i_background_color[1] );
    MP4_GET1BYTE ( p_box->data.p_sample_text->i_background_color[2] );
    MP4_GET1BYTE ( p_box->data.p_sample_text->i_background_color[3] );

    MP4_GET2BYTES( p_box->data.p_sample_text->i_text_box_top );
    MP4_GET2BYTES( p_box->data.p_sample_text->i_text_box_left );
    MP4_GET2BYTES( p_box->data.p_sample_text->i_text_box_bottom );
    MP4_GET2BYTES( p_box->data.p_sample_text->i_text_box_right );

    MP4_GET4BYTES( p_box->data.p_sample_text->i_reserved3 );

    MP4_GET2BYTES( p_box->data.p_sample_text->i_font_id );
    MP4_GET1BYTE ( p_box->data.p_sample_text->i_font_face );
    MP4_GET1BYTE ( p_box->data.p_sample_text->i_font_size );
    MP4_GET4BYTES( p_box->data.p_sample_text->i_font_color );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"tx3g\" in stsd text" );
#endif
    MP4_READBOX_EXIT( 1 );
}