uint32_t seek_callback(void *user_data, uint64_t position)
{
    return fseek((FILE*)user_data, position, SEEK_SET);
}