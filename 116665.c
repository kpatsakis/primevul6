static rpmfi fsmIterFini(rpmfi fi, struct diriter_s *di)
{
    close(di->dirfd);
    di->dirfd = -1;
    return rpmfiFree(fi);
}