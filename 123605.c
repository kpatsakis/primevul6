static void ResetPTStlb( encoder_t *p_enc )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    for( int i = 0; i < SCHRO_PTS_TLB_SIZE; i++ )
    {
        p_sys->pts_tlb[i].b_empty = true;
    }
}