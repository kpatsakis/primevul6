void _WM_AdjustChannelVolumes(struct _mdi *mdi, uint8_t ch) {
    struct _note *nte = mdi->note;
    if (nte != NULL) {
        do {
            if (ch <= 15) {
                if ((nte->noteid >> 8) == ch) {
                    goto _DO_ADJUST;
                }
            } else {
            _DO_ADJUST:
                if (!nte->ignore_chan_events) {
                    _WM_AdjustNoteVolumes(mdi, ch, nte);
                    if (nte->replay) _WM_AdjustNoteVolumes(mdi, ch, nte->replay);
                }
            }
            nte = nte->next;
        } while (nte != NULL);
    }
}