AP_DECLARE(request_rec *) ap_sub_req_lookup_dirent(const apr_finfo_t *dirent,
                                                   const request_rec *r,
                                                   int subtype,
                                                   ap_filter_t *next_filter)
{
    request_rec *rnew;
    int res;
    char *fdir;
    char *udir;

    rnew = make_sub_request(r, next_filter);

    /* Special case: we are looking at a relative lookup in the same directory.
     * This is 100% safe, since dirent->name just came from the filesystem.
     */
    if (r->path_info && *r->path_info) {
        /* strip path_info off the end of the uri to keep it in sync
         * with r->filename, which has already been stripped by directory_walk,
         * merge the dirent->name, and then, if the caller wants us to remerge
         * the original path info, do so.  Note we never fix the path_info back
         * to r->filename, since dir_walk would do so (but we don't expect it
         * to happen in the usual cases)
         */
        udir = apr_pstrdup(rnew->pool, r->uri);
        udir[ap_find_path_info(udir, r->path_info)] = '\0';
        udir = ap_make_dirstr_parent(rnew->pool, udir);

        rnew->uri = ap_make_full_path(rnew->pool, udir, dirent->name);
        if (subtype == AP_SUBREQ_MERGE_ARGS) {
            rnew->uri = ap_make_full_path(rnew->pool, rnew->uri, r->path_info + 1);
            rnew->path_info = apr_pstrdup(rnew->pool, r->path_info);
        }
        rnew->uri = ap_escape_uri(rnew->pool, rnew->uri);
    }
    else {
        udir = ap_make_dirstr_parent(rnew->pool, r->uri);
        rnew->uri = ap_escape_uri(rnew->pool, ap_make_full_path(rnew->pool,
                                                                udir,
                                                                dirent->name));
    }

    fdir = ap_make_dirstr_parent(rnew->pool, r->filename);
    rnew->filename = ap_make_full_path(rnew->pool, fdir, dirent->name);
    if (r->canonical_filename == r->filename) {
        rnew->canonical_filename = rnew->filename;
    }

    /* XXX This is now less relevant; we will do a full location walk
     * these days for this case.  Preserve the apr_stat results, and
     * perhaps we also tag that symlinks were tested and/or found for
     * r->filename.
     */
    rnew->per_dir_config = r->server->lookup_defaults;

    if ((dirent->valid & APR_FINFO_MIN) != APR_FINFO_MIN) {
        /*
         * apr_dir_read isn't very complete on this platform, so
         * we need another apr_stat (with or without APR_FINFO_LINK
         * depending on whether we allow all symlinks here.)  If this
         * is an APR_LNK that resolves to an APR_DIR, then we will rerun
         * everything anyways... this should be safe.
         */
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
    }
    else {
        memcpy(&rnew->finfo, dirent, sizeof(apr_finfo_t));
    }

    if (rnew->finfo.filetype == APR_LNK) {
        /*
         * Resolve this symlink.  We should tie this back to dir_walk's cache
         */
        if ((res = resolve_symlink(rnew->filename, &rnew->finfo,
                                   ap_allow_options(rnew), rnew->pool))
            != OK) {
            rnew->status = res;
            return rnew;
        }
    }

    if (rnew->finfo.filetype == APR_DIR) {
        /* ap_make_full_path and ap_escape_uri overallocated the buffers
         * by one character to help us out here.
         */
        strcat(rnew->filename, "/");
        if (!rnew->path_info || !*rnew->path_info) {
            strcat(rnew->uri, "/");
        }
    }

    /* fill in parsed_uri values
     */
    if (r->args && *r->args && (subtype == AP_SUBREQ_MERGE_ARGS)) {
        ap_parse_uri(rnew, apr_pstrcat(r->pool, rnew->uri, "?",
                                       r->args, NULL));
    }
    else {
        ap_parse_uri(rnew, rnew->uri);
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