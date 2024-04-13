static void update_stat(struct node *node, const struct stat *stbuf)
{
    if (node->cache_valid && (!mtime_eq(stbuf, &node->mtime) ||
                              stbuf->st_size != node->size))
        node->cache_valid = 0;
    node->mtime.tv_sec = stbuf->st_mtime;
    node->mtime.tv_nsec = ST_MTIM_NSEC(stbuf);
    node->size = stbuf->st_size;
    curr_time(&node->stat_updated);
}