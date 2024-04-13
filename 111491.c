void _WM_do_control_channel_notes_off(struct _mdi *mdi,
                                      struct _event_data *data) {
    struct _note *note_data = mdi->note;
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if (mdi->channel[ch].isdrum)
        return;
    if (note_data) {
        do {
            if ((note_data->noteid >> 8) == ch) {
                if (!note_data->hold) {
                    if (note_data->modes & SAMPLE_ENVELOPE) {
                        if (note_data->env < 5) {
                            if (note_data->env_level
                                > note_data->sample->env_target[5]) {
                                note_data->env_inc =
                                -note_data->sample->env_rate[5];
                            } else {
                                note_data->env_inc =
                                note_data->sample->env_rate[5];
                            }
                            note_data->env = 5;
                        }
                    }
                } else {
                    note_data->hold |= HOLD_OFF;
                }
            }
            note_data = note_data->next;
        } while (note_data);
    }
}