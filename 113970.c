static bool ok_file_seek(void *user_data, long count) {
    return fseek((FILE *)user_data, count, SEEK_CUR) == 0;
}