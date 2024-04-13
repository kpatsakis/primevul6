void _WM_do_note_on(struct _mdi *mdi, struct _event_data *data) {
    struct _note *nte;
    struct _note *prev_nte;
    struct _note *nte_array;
    uint32_t freq = 0;
    struct _patch *patch;
    struct _sample *sample;
    uint8_t ch = data->channel;
    uint8_t note = (data->data.value >> 8);
    uint8_t velocity = (data->data.value & 0xFF);

    if (velocity == 0x00) {
        _WM_do_note_off(mdi, data);
        return;
    }

    MIDI_EVENT_DEBUG(__FUNCTION__,ch, data->data.value);

    if (!mdi->channel[ch].isdrum) {
        patch = mdi->channel[ch].patch;
        if (patch == NULL) {
            return;
        }
        freq = _WM_freq_table[(note % 12) * 100] >> (10 - (note / 12));
    } else {
        patch = _WM_get_patch_data(mdi,
                               ((mdi->channel[ch].bank << 8) | note | 0x80));
        if (patch == NULL) {
            return;
        }
        if (patch->note) {
            freq = _WM_freq_table[(patch->note % 12) * 100]
            >> (10 - (patch->note / 12));
        } else {
            freq = _WM_freq_table[(note % 12) * 100] >> (10 - (note / 12));
        }
    }

    sample = _WM_get_sample_data(patch, (freq / 100));
    if (sample == NULL) {
        return;
    }

    nte = &mdi->note_table[0][ch][note];

    if (nte->active) {
        if ((nte->modes & SAMPLE_ENVELOPE) && (nte->env < 3)
            && (!(nte->hold & HOLD_OFF)))
            return;
        nte->replay = &mdi->note_table[1][ch][note];
        nte->env = 6;
        nte->env_inc = -nte->sample->env_rate[6];
        nte = nte->replay;
    } else {
        if (mdi->note_table[1][ch][note].active) {
            if ((nte->modes & SAMPLE_ENVELOPE) && (nte->env < 3)
                && (!(nte->hold & HOLD_OFF)))
                return;
            mdi->note_table[1][ch][note].replay = nte;
            mdi->note_table[1][ch][note].env = 6;
            mdi->note_table[1][ch][note].env_inc =
            -mdi->note_table[1][ch][note].sample->env_rate[6];
        } else {
            nte_array = mdi->note;
            if (nte_array == NULL) {
                mdi->note = nte;
            } else {
                do {
                    prev_nte = nte_array;
                    nte_array = nte_array->next;
                } while (nte_array);
                prev_nte->next = nte;
            }
            nte->active = 1;
            nte->next = NULL;
        }
    }
    nte->noteid = (ch << 8) | note;
    nte->patch = patch;
    nte->sample = sample;
    nte->sample_pos = 0;
    nte->sample_inc = get_inc(mdi, nte);
    nte->velocity = velocity;
    nte->env = 0;
    nte->env_inc = nte->sample->env_rate[0];
    nte->env_level = 0;
    nte->modes = sample->modes;
    nte->hold = mdi->channel[ch].hold;
    nte->replay = NULL;
    nte->is_off = 0;
    nte->ignore_chan_events = 0;
    _WM_AdjustNoteVolumes(mdi, ch, nte);
}