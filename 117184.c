static fuse_ino_t next_id(struct fuse *f)
{
    do {
        f->ctr = (f->ctr + 1) & 0xffffffff;
        if (!f->ctr)
            f->generation ++;
    } while (f->ctr == 0 || f->ctr == FUSE_UNKNOWN_INO ||
             get_node_nocheck(f, f->ctr) != NULL);
    return f->ctr;
}