static int fsmRemove(int dirfd, const char *path, mode_t mode)
{
    return S_ISDIR(mode) ? fsmRmdir(dirfd, path) : fsmUnlink(dirfd, path);
}