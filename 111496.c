void _WM_do_pitch(struct _mdi *mdi, struct _event_data *data) {
    struct _note *note_data = mdi->note;
    uint8_t ch = data->channel;

    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);
    mdi->channel[ch].pitch = data->data.value - 0x2000;

    if (mdi->channel[ch].pitch < 0) {
        mdi->channel[ch].pitch_adjust = mdi->channel[ch].pitch_range
        * mdi->channel[ch].pitch / 8192;
    } else {
        mdi->channel[ch].pitch_adjust = mdi->channel[ch].pitch_range
        * mdi->channel[ch].pitch / 8191;
    }

    if (note_data) {
        do {
            if ((note_data->noteid >> 8) == ch) {
                note_data->sample_inc = get_inc(mdi, note_data);
            }
            note_data = note_data->next;
        } while (note_data);
    }
}