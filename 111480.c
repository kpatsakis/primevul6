void _WM_DynamicVolumeAdjust(struct _mdi *mdi, int32_t *tmp_buffer, uint32_t buffer_used) {

    uint32_t i = 0;
    uint32_t j = 0;

    int8_t peak_set = 0;
    int32_t prev_val = 0;
    uint32_t peak_ofs = 0;
    int32_t peak = mdi->dyn_vol_peak;

    double volume_to_reach = mdi->dyn_vol_to_reach;
    double volume = mdi->dyn_vol;
    double volume_adjust = mdi->dyn_vol_adjust;
    double tmp_output = 0.0;

#define MAX_DYN_VOL 1.0

    for (i = 0; i < buffer_used; i++) {
        if ((i == 0) || (i > peak_ofs)) {
            // Find Next Peak/Troff
            peak_set = 0;
            prev_val = peak;
            peak_ofs = 0;
            for (j = i; j < buffer_used; j++) {
                if (peak_set == 0) {
                    // find what direction the data is going
                    if (prev_val > tmp_buffer[j]) {
                        // Going Down
                        peak_set = -1;
                    } else if (prev_val < tmp_buffer[j]) {
                        // Doing Up
                        peak_set = 1;
                    } else {
                        // No direction, keep looking
                        prev_val = tmp_buffer[j];
                        continue;
                    }
                }

                if (peak_set == 1) {
                    // Data is going up
                    if (peak < tmp_buffer[j]) {
                        peak = tmp_buffer[j];
                        peak_ofs = j;
                    } else if (peak > tmp_buffer[j]) {
                        // Data is starting to go down, we found the peak
                        break;
                    }
                } else { // assume peak_set == -1
                    // Data is going down
                    if (peak > tmp_buffer[j]) {
                        peak = tmp_buffer[j];
                        peak_ofs = j;
                    } else if (peak < tmp_buffer[j]) {
                        // Data is starting to go up, we found the troff
                        break;
                    }
                }

                prev_val = tmp_buffer[j];
            }

            if (peak_set != 0) {
                if (peak_set == 1) {
                    if (peak > 32767) {
                        volume_to_reach = 32767.0 / (double)peak;
                    } else {
                        volume_to_reach = MAX_DYN_VOL;
                    }
                } else { // assume peak_set == -1
                    if (peak < -32768) {
                        volume_to_reach = -32768.0 / (double)peak;
                    } else {
                        volume_to_reach = MAX_DYN_VOL;
                    }
                }
            } else {
                // No peak found, set volume we want to normal
                volume_to_reach = MAX_DYN_VOL;
            }

            if (volume != volume_to_reach) {
                if (volume_to_reach == MAX_DYN_VOL) {
                    // if we want normal volume then adjust to it slower
                    volume_adjust = (volume_to_reach - volume) / ((double)_WM_SampleRate * 0.1);
                } else {
                    // if we want to clamp the volume then adjust quickly
                    volume_adjust = (volume_to_reach - volume) / ((double)_WM_SampleRate * 0.0001);
                }
            }
        }

        // First do we need to do volume adjustments
        if ((volume_adjust != 0.0) && (volume != volume_to_reach)) {
                volume += volume_adjust;
                if (volume_adjust > 0.0) {
                    // if increasing the volume
                    if (volume >= MAX_DYN_VOL) {
                        // we dont boost volume
                        volume = MAX_DYN_VOL;
                        volume_adjust = 0.0;
                    } else if (volume > volume_to_reach) {
                        // we dont want to go above the level we wanted
                        volume = volume_to_reach;
                        volume_adjust = 0.0;
                    }
                } else {
                    // decreasing the volume
                    if (volume < volume_to_reach) {
                        // we dont want to go below the level we wanted
                        volume = volume_to_reach;
                        volume_adjust = 0.0;
                    }
                }
            }

        // adjust buffer volume
        tmp_output = (double)tmp_buffer[i] * volume;
        tmp_buffer[i] = (int32_t)tmp_output;
    }

    // store required values
    mdi->dyn_vol_adjust = volume_adjust;
    mdi->dyn_vol_peak = peak;
    mdi->dyn_vol = volume;
    mdi->dyn_vol_to_reach = volume_to_reach;
}