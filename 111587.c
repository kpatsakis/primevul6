static inline gboolean uc_read_exit_iter(gpointer key, gpointer val,
                                         gpointer data)
{
    uc_ctl_exit_request *req = (uc_ctl_exit_request *)data;

    req->array[req->len++] = *(uint64_t *)key;

    return false;
}