static int lo_change_cred(fuse_req_t req, struct lo_cred *old)
{
    int res;

    old->euid = geteuid();
    old->egid = getegid();

    res = syscall(OURSYS_setresgid, -1, fuse_req_ctx(req)->gid, -1);
    if (res == -1) {
        return errno;
    }

    res = syscall(OURSYS_setresuid, -1, fuse_req_ctx(req)->uid, -1);
    if (res == -1) {
        int errno_save = errno;

        syscall(OURSYS_setresgid, -1, old->egid, -1);
        return errno_save;
    }

    return 0;
}