void _WM_do_aftertouch(struct _mdi *mdi, struct _event_data *data) {
    struct _note *nte;
    uint8_t ch = data->channel;

    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    nte = &mdi->note_table[0][ch][(data->data.value >> 8)];
    if (!nte->active) {
        nte = &mdi->note_table[1][ch][(data->data.value >> 8)];
        if (!nte->active) {
            return;
        }
    }

    nte->velocity = data->data.value & 0xff;
    _WM_AdjustNoteVolumes(mdi, ch, nte);
    if (nte->replay) {
        nte->replay->velocity = data->data.value & 0xff;
        _WM_AdjustNoteVolumes(mdi, ch, nte->replay);
    }
}