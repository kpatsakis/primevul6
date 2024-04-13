msg_sb_start(msgchunk_T *mps)
{
    msgchunk_T *mp = mps;

    while (mp != NULL && mp->sb_prev != NULL && !mp->sb_prev->sb_eol)
	mp = mp->sb_prev;
    return mp;
}