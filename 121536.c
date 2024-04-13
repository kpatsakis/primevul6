void print_results() {
    if (!g_use_seccomp) {
        uint64_t earliest_time = get_time_us();
        for (int i = 0; i < NUM_STAGES; ++i) {
            for (unsigned int j = 0; j < MAX_NUM_THREADS && j < NUM_THREADS; ++j) {
                if (timing[j][i] && timing[j][i] < earliest_time) {
                    earliest_time = timing[j][i];
                }
            }
        }
        for (int i = 0; i < NUM_STAGES; ++i) {
            for (unsigned int j = 0; j < MAX_NUM_THREADS && j < NUM_THREADS; ++j) {
                if (timing[j][i]) {
                    fprintf(stderr,
                            "%s\t(%d)\t%f\n",
                            stage_names[i], j,
                            (timing[j][i] - earliest_time) * 0.000001);
                }
            }
        }
    }
}