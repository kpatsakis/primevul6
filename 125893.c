static void MP4_FreeBox_avcC( MP4_Box_t *p_box )
{
    MP4_Box_data_avcC_t *p_avcC = p_box->data.p_avcC;
    int i;

    if( p_avcC->i_avcC > 0 ) FREENULL( p_avcC->p_avcC );

    if( p_avcC->sps )
    {
        for( i = 0; i < p_avcC->i_sps; i++ )
            FREENULL( p_avcC->sps[i] );
    }
    if( p_avcC->pps )
    {
        for( i = 0; i < p_avcC->i_pps; i++ )
            FREENULL( p_avcC->pps[i] );
    }
    if( p_avcC->i_sps > 0 ) FREENULL( p_avcC->sps );
    if( p_avcC->i_sps > 0 ) FREENULL( p_avcC->i_sps_length );
    if( p_avcC->i_pps > 0 ) FREENULL( p_avcC->pps );
    if( p_avcC->i_pps > 0 ) FREENULL( p_avcC->i_pps_length );
}