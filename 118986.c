AP_DECLARE(int) ap_run_sub_req(request_rec *r)
{
    int retval = DECLINED;
    /* Run the quick handler if the subrequest is not a dirent or file
     * subrequest
     */
    if (!(r->filename && r->finfo.filetype != APR_NOFILE)) {
        retval = ap_run_quick_handler(r, 0);
    }
    if (retval != OK) {
        retval = ap_invoke_handler(r);
        if (retval == DONE) {
            retval = OK;
        }
    }
    ap_finalize_sub_req_protocol(r);
    return retval;
}