ok_jpg ok_jpg_read_from_input(ok_jpg_decode_flags decode_flags,
                              ok_jpg_input input_callbacks, void *input_callbacks_user_data,
                              ok_jpg_allocator allocator, void *allocator_user_data) {
    ok_jpg jpg = { 0 };
    ok_jpg_decode(&jpg, decode_flags, input_callbacks, input_callbacks_user_data,
                  allocator, allocator_user_data);
    return jpg;
}