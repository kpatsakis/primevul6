static void MP4_FreeBox_esds( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_esds->es_descriptor.psz_URL );
    if( p_box->data.p_esds->es_descriptor.p_decConfigDescr )
    {
        FREENULL( p_box->data.p_esds->es_descriptor.p_decConfigDescr->p_decoder_specific_info );
        FREENULL( p_box->data.p_esds->es_descriptor.p_decConfigDescr );
    }
}