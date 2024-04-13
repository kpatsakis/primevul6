AP_DECLARE(request_rec *) ap_sub_req_lookup_file(const char *new_file,
                                                 const request_rec *r,
                                                 ap_filter_t *next_filter)
{
    request_rec *rnew;
    int res;
    char *fdir;
    apr_size_t fdirlen;

    rnew = make_sub_request(r, next_filter);

    fdir = ap_make_dirstr_parent(rnew->pool, r->filename);
    fdirlen = strlen(fdir);

    /* Translate r->filename, if it was canonical, it stays canonical
     */
    if (r->canonical_filename == r->filename) {
        rnew->canonical_filename = (char*)(1);
    }

    if (apr_filepath_merge(&rnew->filename, fdir, new_file,
                           APR_FILEPATH_TRUENAME, rnew->pool) != APR_SUCCESS) {
        rnew->status = HTTP_FORBIDDEN;
        return rnew;
    }

    if (rnew->canonical_filename) {
        rnew->canonical_filename = rnew->filename;
    }

    /*
     * Check for a special case... if there are no '/' characters in new_file
     * at all, and the path was the same, then we are looking at a relative
     * lookup in the same directory.  Fixup the URI to match.
     */

    if (strncmp(rnew->filename, fdir, fdirlen) == 0
        && rnew->filename[fdirlen]
        && ap_strchr_c(rnew->filename + fdirlen, '/') == NULL) {
        apr_status_t rv;
        if (ap_allow_options(rnew) & OPT_SYM_LINKS) {
            if (((rv = apr_stat(&rnew->finfo, rnew->filename,
                                APR_FINFO_MIN, rnew->pool)) != APR_SUCCESS)
                && (rv != APR_INCOMPLETE)) {
                rnew->finfo.filetype = APR_NOFILE;
            }
        }
        else {
            if (((rv = apr_stat(&rnew->finfo, rnew->filename,
                                APR_FINFO_LINK | APR_FINFO_MIN,
                                rnew->pool)) != APR_SUCCESS)
                && (rv != APR_INCOMPLETE)) {
                rnew->finfo.filetype = APR_NOFILE;
            }
        }

        if (r->uri && *r->uri) {
            char *udir = ap_make_dirstr_parent(rnew->pool, r->uri);
            rnew->uri = ap_make_full_path(rnew->pool, udir,
                                          rnew->filename + fdirlen);
            ap_parse_uri(rnew, rnew->uri);    /* fill in parsed_uri values */
        }
        else {
            ap_parse_uri(rnew, new_file);        /* fill in parsed_uri values */
            rnew->uri = apr_pstrdup(rnew->pool, "");
        }
    }
    else {
        /* XXX: @@@: What should be done with the parsed_uri values?
         * We would be better off stripping down to the 'common' elements
         * of the path, then reassembling the URI as best as we can.
         */
        ap_parse_uri(rnew, new_file);        /* fill in parsed_uri values */
        /*
         * XXX: this should be set properly like it is in the same-dir case
         * but it's actually sometimes to impossible to do it... because the
         * file may not have a uri associated with it -djg
         */
        rnew->uri = apr_pstrdup(rnew->pool, "");
    }

    /* We cannot return NULL without violating the API. So just turn this
     * subrequest into a 500. */
    if (ap_is_recursion_limit_exceeded(r)) {
        rnew->status = HTTP_INTERNAL_SERVER_ERROR;
        return rnew;
    }

    if ((res = ap_process_request_internal(rnew))) {
        rnew->status = res;
    }

    return rnew;
}