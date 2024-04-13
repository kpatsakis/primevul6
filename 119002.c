static request_rec *make_sub_request(const request_rec *r,
                                     ap_filter_t *next_filter)
{
    apr_pool_t *rrp;
    request_rec *rnew;

    apr_pool_create(&rrp, r->pool);
    apr_pool_tag(rrp, "subrequest");
    rnew = apr_pcalloc(rrp, sizeof(request_rec));
    rnew->pool = rrp;

    rnew->hostname       = r->hostname;
    rnew->request_time   = r->request_time;
    rnew->connection     = r->connection;
    rnew->server         = r->server;
    rnew->log            = r->log;

    rnew->request_config = ap_create_request_config(rnew->pool);

    /* Start a clean config from this subrequest's vhost.  Optimization in
     * Location/File/Dir walks from the parent request assure that if the
     * config blocks of the subrequest match the parent request, no merges
     * will actually occur (and generally a minimal number of merges are
     * required, even if the parent and subrequest aren't quite identical.)
     */
    rnew->per_dir_config = r->server->lookup_defaults;

    rnew->htaccess = r->htaccess;
    rnew->allowed_methods = ap_make_method_list(rnew->pool, 2);

    /* make a copy of the allowed-methods list */
    ap_copy_method_list(rnew->allowed_methods, r->allowed_methods);

    /* start with the same set of output filters */
    if (next_filter) {
        /* while there are no input filters for a subrequest, we will
         * try to insert some, so if we don't have valid data, the code
         * will seg fault.
         */
        rnew->input_filters = r->input_filters;
        rnew->proto_input_filters = r->proto_input_filters;
        rnew->output_filters = next_filter;
        rnew->proto_output_filters = r->proto_output_filters;
        ap_add_output_filter_handle(ap_subreq_core_filter_handle,
                                    NULL, rnew, rnew->connection);
    }
    else {
        /* If NULL - we are expecting to be internal_fast_redirect'ed
         * to this subrequest - or this request will never be invoked.
         * Ignore the original request filter stack entirely, and
         * drill the input and output stacks back to the connection.
         */
        rnew->proto_input_filters = r->proto_input_filters;
        rnew->proto_output_filters = r->proto_output_filters;

        rnew->input_filters = r->proto_input_filters;
        rnew->output_filters = r->proto_output_filters;
    }

    rnew->useragent_addr = r->useragent_addr;
    rnew->useragent_ip = r->useragent_ip;

    /* no input filters for a subrequest */

    ap_set_sub_req_protocol(rnew, r);

    /* We have to run this after we fill in sub req vars,
     * or the r->main pointer won't be setup
     */
    ap_run_create_request(rnew);

    /* Begin by presuming any module can make its own path_info assumptions,
     * until some module interjects and changes the value.
     */
    rnew->used_path_info = AP_REQ_DEFAULT_PATH_INFO;

    /* Pass on the kept body (if any) into the new request. */
    rnew->kept_body = r->kept_body;

    return rnew;
}