static int MP4_ReadBox_Metadata( stream_t *p_stream, MP4_Box_t *p_box )
{
    const uint8_t *p_peek;
    if ( stream_Peek( p_stream, &p_peek, 16 ) < 16 )
        return 0;
    if ( stream_Read( p_stream, NULL, 8 ) < 8 )
        return 0;
    return MP4_ReadBoxContainerChildren( p_stream, p_box, ATOM_data );
}