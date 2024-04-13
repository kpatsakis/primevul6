static void MP4_FreeBox_uuid( MP4_Box_t *p_box )
{
    if( !CmpUUID( &p_box->i_uuid, &TfrfBoxUUID ) )
        return MP4_FreeBox_tfrf( p_box );
    if( !CmpUUID( &p_box->i_uuid, &TfxdBoxUUID ) )
        return MP4_FreeBox_Common( p_box );
    if( !CmpUUID( &p_box->i_uuid, &SmooBoxUUID ) )
        return MP4_FreeBox_Common( p_box );
    if( !CmpUUID( &p_box->i_uuid, &StraBoxUUID ) )
        return MP4_FreeBox_stra( p_box );
}