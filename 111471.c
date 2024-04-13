void _WM_do_control_channel_hold(struct _mdi *mdi, struct _event_data *data) {
    struct _note *note_data = mdi->note;
    uint8_t ch = data->channel;
    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if (data->data.value > 63) {
        mdi->channel[ch].hold = 1;
    } else {
        mdi->channel[ch].hold = 0;
        if (note_data) {
            do {
                if ((note_data->noteid >> 8) == ch) {
                    if (note_data->hold & HOLD_OFF) {
                        if (note_data->modes & SAMPLE_ENVELOPE) {
                            if (note_data->modes & SAMPLE_CLAMPED) {
                                if (note_data->env < 5) {
                                    note_data->env = 5;
                                    if (note_data->env_level
                                        > note_data->sample->env_target[5]) {
                                        note_data->env_inc =
                                        -note_data->sample->env_rate[5];
                                    } else {
                                        note_data->env_inc =
                                        note_data->sample->env_rate[5];
                                    }
                                }
                            /*
                            } else if (note_data->modes & SAMPLE_SUSTAIN) {
                                if (note_data->env < 3) {
                                    note_data->env = 3;
                                    if (note_data->env_level
                                        > note_data->sample->env_target[3]) {
                                        note_data->env_inc =
                                        -note_data->sample->env_rate[3];
                                    } else {
                                        note_data->env_inc =
                                        note_data->sample->env_rate[3];
                                    }
                                }
                             */
                             } else if (note_data->env < 3) {
                                note_data->env = 3;
                                if (note_data->env_level
                                    > note_data->sample->env_target[3]) {
                                    note_data->env_inc =
                                    -note_data->sample->env_rate[3];
                                } else {
                                    note_data->env_inc =
                                    note_data->sample->env_rate[3];
                                }
                            }
                        } else {
                            if (note_data->modes & SAMPLE_LOOP) {
                                note_data->modes ^= SAMPLE_LOOP;
                            }
                            note_data->env_inc = 0;
                        }
                    }
                    note_data->hold = 0x00;
                }
                note_data = note_data->next;
            } while (note_data);
        }
    }
}