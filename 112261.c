uint32_t read_callback(void *user_data, void *buffer, uint32_t length)
{
    return fread(buffer, 1, length, (FILE*)user_data);
}