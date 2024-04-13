void check_decompression_memory_bound_ok() {
    if (g_decompression_memory_bound) {
        size_t adjustment = 0;
        if (!uninit_g_zlib_0_writer) {
            adjustment = 8192; // add an extra 8kb if we're decoding zlib
        }
        if (decompression_memory_bound() > g_decompression_memory_bound + adjustment) {
            custom_exit(ExitCode::TOO_MUCH_MEMORY_NEEDED);
        }
    }
}