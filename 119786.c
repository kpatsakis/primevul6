static ssize_t lo_add_fd_mapping(fuse_req_t req, int fd)
{
    struct lo_map_elem *elem;

    elem = lo_map_alloc_elem(&lo_data(req)->fd_map);
    if (!elem) {
        return -1;
    }

    elem->fd = fd;
    return elem - lo_data(req)->fd_map.elems;
}