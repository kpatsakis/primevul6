static void StorePicturePTS( encoder_t *p_enc, uint32_t u_pnum, mtime_t i_pts )
{
    encoder_sys_t *p_sys = p_enc->p_sys;

    for( int i = 0; i<SCHRO_PTS_TLB_SIZE; i++ )
    {
        if( p_sys->pts_tlb[i].b_empty )
        {
            p_sys->pts_tlb[i].u_pnum = u_pnum;
            p_sys->pts_tlb[i].i_pts = i_pts;
            p_sys->pts_tlb[i].b_empty = false;

            return;
        }
    }

    msg_Err( p_enc, "Could not store PTS %"PRId64" for frame %u", i_pts, u_pnum );
}