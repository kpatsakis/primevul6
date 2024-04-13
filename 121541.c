size_t decompression_memory_bound() {
    if (ofiletype == UJG || filetype == UJG) {
        return 0;
    }
    size_t cumulative_buffer_size = 0;
    size_t streaming_buffer_size = 0;
    size_t current_run_size = 0;
    for (int i = 0; i < colldata.get_num_components(); ++i) {
        size_t streaming_size = 
            colldata.block_width(i)
            * 2 * NUM_THREADS * 64 * sizeof(uint16_t);
        size_t frame_buffer_size = colldata.component_size_allocated(i);
        if (cs_cmpc != colldata.get_num_components() || jpegtype != 1) {
            streaming_size = frame_buffer_size;
        } else if (filetype != JPEG) {
            if (!g_threaded) {
                frame_buffer_size = colldata.block_width(i) * 2 * 64 * sizeof(uint16_t);

            } else {
                frame_buffer_size = streaming_size;
            }
        }
        cumulative_buffer_size += frame_buffer_size;
        streaming_buffer_size += streaming_size;
    }
    current_run_size = cumulative_buffer_size;

    size_t bit_writer_augmentation = 0;
    if (g_allow_progressive) {
        for (size_t cur_size = jpgfilesize - 1; cur_size; cur_size >>=1) {
            bit_writer_augmentation |= cur_size;
        }
        bit_writer_augmentation += 1; // this is used to compute the buffer size of the abit_writer for writing
    }
    size_t garbage_augmentation = 0;
    for (size_t cur_size = hdrs - 1; cur_size; cur_size >>=1) {
        garbage_augmentation |= cur_size;
    }
    garbage_augmentation += 1; // this is used to compute the buffer size of the abit_writer for writing
    int non_preloaded_mux = 4096 * 1024 + 131072; // only 1 thread hence only one extra 131072
    size_t decode_header_needed_size = hdrs + zlib_hdrs * 3;
    if (zlib_hdrs && zlib_hdrs * 2 < hdrs) {
        size_t doubled = zlib_hdrs * 2;
        do {
            decode_header_needed_size += doubled;
            doubled *= 2;
        } while (doubled < (size_t)hdrs);
    }
    size_t single_threaded_model_bonus = 0;
    size_t single_threaded_buffer_bonus = 0; //the threads have to save their output to 3/4 of the jpeg before writing it
    if (g_decoder) {
        single_threaded_model_bonus += g_decoder->get_model_worker_memory_usage();
    } else if (g_encoder) {
        single_threaded_model_bonus += g_encoder->get_decode_model_worker_memory_usage();
    }
    if (filetype != JPEG && !g_threaded) {
        single_threaded_buffer_bonus += jpgfilesize;
    }
    size_t abit_writer = 0;
    if (g_allow_progressive) {

        if (zlib_hdrs * 3 < ABIT_WRITER_PRELOAD * 2 + 64) {
            if (zlib_hdrs * 3 < ABIT_WRITER_PRELOAD + 64) {
                abit_writer += ABIT_WRITER_PRELOAD * 2 + 64;// these can't be reused memory
            } else {
                abit_writer += ABIT_WRITER_PRELOAD + 64;// these can't be reused
            }
        }
    } else {
        abit_writer += 65536 + 64;
    }
    if (g_allow_progressive &&
        jpgfilesize > ABIT_WRITER_PRELOAD) {
        // we currently buffer the whole jpeg in memory while streaming out
        abit_writer += 3 * jpgfilesize;
    }
    size_t total = Sirikata::memmgr_size_allocated();
    ptrdiff_t decom_memory_bound = total;
    decom_memory_bound -= current_run_size;
    decom_memory_bound += streaming_buffer_size;
    decom_memory_bound -= single_threaded_model_bonus;
    decom_memory_bound += single_threaded_buffer_bonus;
    if (decom_memory_bound < 1){
        decom_memory_bound = 1;
    }
    if (filetype == JPEG) {
        decom_memory_bound = streaming_buffer_size
            + abit_writer + jpgfilesize + sizeof(ProbabilityTablesBase)
            + garbage_augmentation + decode_header_needed_size + non_preloaded_mux;
    }
    return decom_memory_bound;
}