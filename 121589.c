void prep_for_new_file() {
    r_bitcount = 0;
    if (prefix_grbgdata) {
        aligned_dealloc(prefix_grbgdata);
        prefix_grbgdata = NULL;
    }
    if (grbgdata && grbgdata != &EOI[0]) {
        aligned_dealloc(grbgdata);
        grbgdata = NULL;
    }

    prefix_grbs = 0;
    reset_buffers();
    auto cur_num_threads = read_fixed_ujpg_header();
    always_assert(cur_num_threads <= NUM_THREADS); // this is an invariant we need to maintain
    str_out->prep_for_new_file();
}