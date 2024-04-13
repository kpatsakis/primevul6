static void ok_jpg_set_error(ok_jpg *jpg, ok_jpg_error error_code) {
    if (jpg) {
        jpg->width = 0;
        jpg->height = 0;
        jpg->error_code = error_code;
    }
}