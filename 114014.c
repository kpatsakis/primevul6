static size_t ok_file_read(void *user_data, uint8_t *buffer, size_t length) {
    return fread(buffer, 1, length, (FILE *)user_data);
}