static void MP4_FreeBox_hvcC(MP4_Box_t *p_box )
{
    MP4_Box_data_hvcC_t *p_hvcC =  p_box->data.p_hvcC;
    if( p_hvcC->i_hvcC > 0 ) FREENULL( p_hvcC->p_hvcC) ;
}