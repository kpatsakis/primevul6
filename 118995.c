AP_DECLARE(request_rec *) ap_sub_req_method_uri(const char *method,
                                                const char *new_uri,
                                                const request_rec *r,
                                                ap_filter_t *next_filter)
{
    request_rec *rnew;
    /* Initialise res, to avoid a gcc warning */
    int res = HTTP_INTERNAL_SERVER_ERROR;
    char *udir;

    rnew = make_sub_request(r, next_filter);

    /* would be nicer to pass "method" to ap_set_sub_req_protocol */
    rnew->method = method;
    rnew->method_number = ap_method_number_of(method);

    if (new_uri[0] == '/') {
        ap_parse_uri(rnew, new_uri);
    }
    else {
        udir = ap_make_dirstr_parent(rnew->pool, r->uri);
        udir = ap_escape_uri(rnew->pool, udir);    /* re-escape it */
        ap_parse_uri(rnew, ap_make_full_path(rnew->pool, udir, new_uri));
    }

    /* We cannot return NULL without violating the API. So just turn this
     * subrequest into a 500 to indicate the failure. */
    if (ap_is_recursion_limit_exceeded(r)) {
        rnew->status = HTTP_INTERNAL_SERVER_ERROR;
        return rnew;
    }

    /* lookup_uri
     * If the content can be served by the quick_handler, we can
     * safely bypass request_internal processing.
     *
     * If next_filter is NULL we are expecting to be
     * internal_fast_redirect'ed to the subrequest, or the subrequest will
     * never be invoked. We need to make sure that the quickhandler is not
     * invoked by any lookups. Since an internal_fast_redirect will always
     * occur too late for the quickhandler to handle the request.
     */
    if (next_filter) {
        res = ap_run_quick_handler(rnew, 1);
    }

    if (next_filter == NULL || res != OK) {
        if ((res = ap_process_request_internal(rnew))) {
            rnew->status = res;
        }
    }

    return rnew;
}