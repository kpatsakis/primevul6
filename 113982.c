ok_jpg ok_jpg_read_with_allocator(FILE *file, ok_jpg_decode_flags decode_flags,
                                  ok_jpg_allocator allocator, void *allocator_user_data) {
    ok_jpg jpg = { 0 };
    if (file) {
        ok_jpg_decode(&jpg, decode_flags, OK_JPG_FILE_INPUT, file, allocator, allocator_user_data);
    } else {
        ok_jpg_error(&jpg, OK_JPG_ERROR_API, "File not found");
    }
    return jpg;
}