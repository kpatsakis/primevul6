static int MP4_ReadBox_uuid( stream_t *p_stream, MP4_Box_t *p_box )
{
    if( !CmpUUID( &p_box->i_uuid, &TfrfBoxUUID ) )
        return MP4_ReadBox_tfrf( p_stream, p_box );
    if( !CmpUUID( &p_box->i_uuid, &TfxdBoxUUID ) )
        return MP4_ReadBox_tfxd( p_stream, p_box );
    if( !CmpUUID( &p_box->i_uuid, &SmooBoxUUID ) )
        return MP4_ReadBoxContainer( p_stream, p_box );
    if( !CmpUUID( &p_box->i_uuid, &StraBoxUUID ) )
        return MP4_ReadBox_stra( p_stream, p_box );

    msg_Warn( p_stream, "Unknown uuid type box" );
    return 1;
}