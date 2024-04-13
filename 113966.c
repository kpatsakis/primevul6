static void ok_jpg_decode(ok_jpg *jpg, ok_jpg_decode_flags decode_flags,
                          ok_jpg_input input, void *input_user_data,
                          ok_jpg_allocator allocator, void *allocator_user_data) {
    if (!input.read || !input.seek) {
        ok_jpg_error(jpg, OK_JPG_ERROR_API,
                     "Invalid argument: read_func and seek_func must not be NULL");
        return;
    }
    
    if (!allocator.alloc || !allocator.free) {
        ok_jpg_error(jpg, OK_JPG_ERROR_API,
                     "Invalid argument: allocator alloc and free functions must not be NULL");
        return;
    }

    ok_jpg_decoder *decoder = allocator.alloc(allocator_user_data, sizeof(ok_jpg_decoder));
    if (!decoder) {
        ok_jpg_error(jpg, OK_JPG_ERROR_ALLOCATION, "Couldn't allocate decoder.");
        return;
    }
    memset(decoder, 0, sizeof(ok_jpg_decoder));

    decoder->jpg = jpg;
    decoder->input = input;
    decoder->input_user_data = input_user_data;
    decoder->allocator = allocator;
    decoder->allocator_user_data = allocator_user_data;
    decoder->color_rgba = (decode_flags & OK_JPG_COLOR_FORMAT_BGRA) == 0;
    decoder->flip_y = (decode_flags & OK_JPG_FLIP_Y) != 0;
    decoder->info_only = (decode_flags & OK_JPG_INFO_ONLY) != 0;

    ok_jpg_decode2(decoder);

    for (int i = 0; i < MAX_COMPONENTS; i++) {
        allocator.free(allocator_user_data, decoder->components[i].blocks);
    }
    allocator.free(allocator_user_data, decoder);
}