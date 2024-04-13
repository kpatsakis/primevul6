void _WM_do_channel_pressure(struct _mdi *mdi, struct _event_data *data) {
    uint8_t ch = data->channel;
    struct _note *note_data = mdi->note;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    mdi->channel[ch].pressure = data->data.value;

    while (note_data) {
        if (!note_data->ignore_chan_events) {
            if ((note_data->noteid >> 8) == ch) {
                note_data->velocity = data->data.value & 0xff;
                _WM_AdjustNoteVolumes(mdi, ch, note_data);
                if (note_data->replay) {
                    note_data->replay->velocity = data->data.value & 0xff;
                    _WM_AdjustNoteVolumes(mdi, ch, note_data->replay);
                }
            }
        }
        note_data = note_data->next;
    }
}