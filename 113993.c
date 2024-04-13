ok_jpg ok_jpg_read(FILE *file, ok_jpg_decode_flags decode_flags) {
    return ok_jpg_read_with_allocator(file, decode_flags, OK_JPG_DEFAULT_ALLOCATOR, NULL);
}