bool check_value_range( void )
{
    int bad_cmp = 0, bad_bpos = 0, bad_dpos = 0;
    bool bad_colldata = false;
    // out of range should never happen with unmodified JPEGs
    for (int cmp = 0; cmp < cmpc && cmp < 4; cmp++ ) {
        int absmax[64];
        for (int bpos = 0; bpos < 64; bpos++ ) {
            absmax[zigzag_to_aligned.at(bpos)] = MAX_V( cmp, bpos );
        }
        for (int dpos = 0; dpos < cmpnfo[cmp].bc && dpos <= max_dpos[cmp] ; dpos++ ) {
            const int16_t * raw_data = colldata.block_nosync((BlockType)cmp, dpos).raw_data();
            for (int aligned_pos = 0; aligned_pos < 64; ++aligned_pos, ++raw_data) {
                if ((*raw_data) > absmax[aligned_pos] ||
                    (*raw_data) < -absmax[aligned_pos]) {
                    int bpos = aligned_to_zigzag.at(aligned_pos);
                    if (!early_eof_encountered) {
                        fprintf( stderr, "value out of range error: cmp%i, frq%i, val %i, max %i",
                             cmp, bpos, colldata.at_nosync((BlockType)cmp,bpos,dpos), absmax[aligned_pos] );
                        errorlevel.store(2);
                        return false;
                    }
                    bad_cmp = cmp;
                    bad_bpos = bpos;
                    bad_dpos = dpos;
                    colldata.set((BlockType)bad_cmp, bad_bpos, bad_dpos) = 0; // zero this puppy out
                    bad_colldata = true;
                }
            }
        }
    }
    if (bad_colldata) {
        colldata.set((BlockType)bad_cmp, bad_bpos, bad_dpos) = 0; // zero this puppy out
    }
    return true;
}