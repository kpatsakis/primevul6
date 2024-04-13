static int MP4_ReadBox_cprt( stream_t *p_stream, MP4_Box_t *p_box )
{
    uint16_t i_language;
    bool b_mac;

    MP4_READBOX_ENTER( MP4_Box_data_cprt_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_cprt );

    MP4_GET2BYTES( i_language );
    decodeQtLanguageCode( i_language, p_box->data.p_cprt->rgs_language, &b_mac );

    MP4_GETSTRINGZ( p_box->data.p_cprt->psz_notice );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"cprt\" language %3.3s notice %s",
                      p_box->data.p_cprt->rgs_language,
                      p_box->data.p_cprt->psz_notice );

#endif
    MP4_READBOX_EXIT( 1 );
}