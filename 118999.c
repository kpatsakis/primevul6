AP_DECLARE(void) ap_update_mtime(request_rec *r, apr_time_t dependency_mtime)
{
    if (r->mtime < dependency_mtime) {
        r->mtime = dependency_mtime;
    }
}