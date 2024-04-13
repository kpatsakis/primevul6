void _WM_do_note_off_extra(struct _note *nte) {

    MIDI_EVENT_DEBUG(__FUNCTION__,0, 0);
    nte->is_off = 0;
        {
        if (!(nte->modes & SAMPLE_ENVELOPE)) {
            if (nte->modes & SAMPLE_LOOP) {
                nte->modes ^= SAMPLE_LOOP;
            }
            nte->env_inc = 0;

        } else if (nte->hold) {
            nte->hold |= HOLD_OFF;
/*
        } else if (nte->modes & SAMPLE_SUSTAIN) {
            if (nte->env < 3) {
                nte->env = 3;
                if (nte->env_level > nte->sample->env_target[3]) {
                    nte->env_inc = -nte->sample->env_rate[3];
                } else {
                    nte->env_inc = nte->sample->env_rate[3];
                }
            }
*/
        } else if (nte->modes & SAMPLE_CLAMPED) {
            if (nte->env < 5) {
                nte->env = 5;
                if (nte->env_level > nte->sample->env_target[5]) {
                    nte->env_inc = -nte->sample->env_rate[5];
                } else {
                    nte->env_inc = nte->sample->env_rate[5];
                }
            }
        } else if (nte->env < 3) {
            nte->env = 3;
            if (nte->env_level > nte->sample->env_target[3]) {
                nte->env_inc = -nte->sample->env_rate[3];
            } else {
                nte->env_inc = nte->sample->env_rate[3];
            }
        }
    }
}