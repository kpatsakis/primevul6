static int64_t get_file_size(int fd)
{
    int64_t size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        return -errno;
    }
    return size;
}