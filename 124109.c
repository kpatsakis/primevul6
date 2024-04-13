static int lua_read_body(request_rec *r, const char **rbuf, apr_off_t *size,
        apr_off_t maxsize)
{
    int rc = OK;

    if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR))) {
        return (rc);
    }
    if (ap_should_client_block(r)) {

        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        char         argsbuffer[HUGE_STRING_LEN];
        apr_off_t    rsize, len_read, rpos = 0;
        apr_off_t length = r->remaining;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        if (maxsize != 0 && length > maxsize) {
            return APR_EINCOMPLETE; /* Only room for incomplete data chunk :( */
        }
        *rbuf = (const char *) apr_pcalloc(r->pool, (apr_size_t) (length + 1));
        *size = length;
        while ((len_read = ap_get_client_block(r, argsbuffer, sizeof(argsbuffer))) > 0) {
            if ((rpos + len_read) > length) {
                rsize = length - rpos;
            }
            else {
                rsize = len_read;
            }

            memcpy((char *) *rbuf + rpos, argsbuffer, (size_t) rsize);
            rpos += rsize;
        }
    }

    return (rc);
}