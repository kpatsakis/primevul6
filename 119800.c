static void lo_forget_multi(fuse_req_t req, size_t count,
                            struct fuse_forget_data *forgets)
{
    int i;

    for (i = 0; i < count; i++) {
        lo_forget_one(req, forgets[i].ino, forgets[i].nlookup);
    }
    fuse_reply_none(req);
}