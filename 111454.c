void _WM_do_note_off(struct _mdi *mdi, struct _event_data *data) {
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

    if ((mdi->channel[ch].isdrum) && (!(nte->modes & SAMPLE_LOOP))) {
        return;
    }

    if ((nte->modes & SAMPLE_ENVELOPE) && (nte->env == 0)) {
        // This is a fix for notes that end before the
        // initial step of the envelope has completed
        // making it impossible to hear them at times.
        nte->is_off = 1;
    } else {
        _WM_do_note_off_extra(nte);
    }
}