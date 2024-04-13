AP_DECLARE(int) ap_directory_walk(request_rec *r)
{
    ap_conf_vector_t *now_merged = NULL;
    core_server_config *sconf =
        ap_get_core_module_config(r->server->module_config);
    ap_conf_vector_t **sec_ent = (ap_conf_vector_t **) sconf->sec_dir->elts;
    int num_sec = sconf->sec_dir->nelts;
    walk_cache_t *cache;
    char *entry_dir;
    apr_status_t rv;
    int cached;

    /* XXX: Better (faster) tests needed!!!
     *
     * "OK" as a response to a real problem is not _OK_, but to allow broken
     * modules to proceed, we will permit the not-a-path filename to pass the
     * following two tests.  This behavior may be revoked in future versions
     * of Apache.  We still must catch it later if it's heading for the core
     * handler.  Leave INFO notes here for module debugging.
     */
    if (r->filename == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00029)
                      "Module bug?  Request filename is missing for URI %s",
                      r->uri);
       return OK;
    }

    /* Canonicalize the file path without resolving filename case or aliases
     * so we can begin by checking the cache for a recent directory walk.
     * This call will ensure we have an absolute path in the same pass.
     */
    if ((rv = apr_filepath_merge(&entry_dir, NULL, r->filename,
                                 APR_FILEPATH_NOTRELATIVE, r->pool))
                  != APR_SUCCESS) {
        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00030)
                      "Module bug?  Request filename path %s is invalid or "
                      "or not absolute for uri %s",
                      r->filename, r->uri);
        return OK;
    }

    /* XXX Notice that this forces path_info to be canonical.  That might
     * not be desired by all apps.  However, some of those same apps likely
     * have significant security holes.
     */
    r->filename = entry_dir;

    cache = prep_walk_cache(AP_NOTE_DIRECTORY_WALK, r);
    cached = (cache->cached != NULL);

    /* If this is not a dirent subrequest with a preconstructed
     * r->finfo value, then we can simply stat the filename to
     * save burning mega-cycles with unneeded stats - if this is
     * an exact file match.  We don't care about failure... we
     * will stat by component failing this meager attempt.
     *
     * It would be nice to distinguish APR_ENOENT from other
     * types of failure, such as APR_ENOTDIR.  We can do something
     * with APR_ENOENT, knowing that the path is good.
     */
    if (r->finfo.filetype == APR_NOFILE || r->finfo.filetype == APR_LNK) {
        rv = ap_run_dirwalk_stat(&r->finfo, r, APR_FINFO_MIN);

        /* some OSs will return APR_SUCCESS/APR_REG if we stat
         * a regular file but we have '/' at the end of the name;
         *
         * other OSs will return APR_ENOTDIR for that situation;
         *
         * handle it the same everywhere by simulating a failure
         * if it looks like a directory but really isn't
         *
         * Also reset if the stat failed, just for safety.
         */
        if ((rv != APR_SUCCESS) ||
            (r->finfo.filetype != APR_NOFILE &&
             (r->finfo.filetype != APR_DIR) &&
             (r->filename[strlen(r->filename) - 1] == '/'))) {
             r->finfo.filetype = APR_NOFILE; /* forget what we learned */
        }
    }

    if (r->finfo.filetype == APR_REG) {
        entry_dir = ap_make_dirstr_parent(r->pool, entry_dir);
    }
    else if (r->filename[strlen(r->filename) - 1] != '/') {
        entry_dir = apr_pstrcat(r->pool, r->filename, "/", NULL);
    }

    /* If we have a file already matches the path of r->filename,
     * and the vhost's list of directory sections hasn't changed,
     * we can skip rewalking the directory_walk entries.
     */
    if (cached
        && ((r->finfo.filetype == APR_REG)
            || ((r->finfo.filetype == APR_DIR)
                && (!r->path_info || !*r->path_info)))
        && (cache->dir_conf_tested == sec_ent)
        && (strcmp(entry_dir, cache->cached) == 0)) {
        int familiar = 0;

        /* Well this looks really familiar!  If our end-result (per_dir_result)
         * didn't change, we have absolutely nothing to do :)
         * Otherwise (as is the case with most dir_merged/file_merged requests)
         * we must merge our dir_conf_merged onto this new r->per_dir_config.
         */
        if (r->per_dir_config == cache->per_dir_result) {
            familiar = 1;
        }

        if (r->per_dir_config == cache->dir_conf_merged) {
            r->per_dir_config = cache->per_dir_result;
            familiar = 1;
        }

        if (familiar) {
            apr_finfo_t thisinfo;
            int res;
            allow_options_t opts;
            core_dir_config *this_dir;

            this_dir = ap_get_core_module_config(r->per_dir_config);
            opts = this_dir->opts;
            /*
             * If Symlinks are allowed in general we do not need the following
             * check.
             */
            if (!(opts & OPT_SYM_LINKS)) {
                rv = ap_run_dirwalk_stat(&thisinfo, r,
                                         APR_FINFO_MIN | APR_FINFO_NAME | APR_FINFO_LINK);
                /*
                 * APR_INCOMPLETE is as fine as result as APR_SUCCESS as we
                 * have added APR_FINFO_NAME to the wanted parameter of
                 * apr_stat above. On Unix platforms this means that apr_stat
                 * is always going to return APR_INCOMPLETE in the case that
                 * the call to the native stat / lstat did not fail.
                 */
                if ((rv != APR_INCOMPLETE) && (rv != APR_SUCCESS)) {
                    /*
                     * This should never happen, because we did a stat on the
                     * same file, resolving a possible symlink several lines
                     * above. Therefore do not make a detailed analysis of rv
                     * in this case for the reason of the failure, just bail out
                     * with a HTTP_FORBIDDEN in case we hit a race condition
                     * here.
                     */
                    ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00031)
                                  "access to %s failed; stat of '%s' failed.",
                                  r->uri, r->filename);
                    return r->status = HTTP_FORBIDDEN;
                }
                if (thisinfo.filetype == APR_LNK) {
                    /* Is this a possibly acceptable symlink? */
                    if ((res = resolve_symlink(r->filename, &thisinfo,
                                               opts, r->pool)) != OK) {
                        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00032)
                                      "Symbolic link not allowed "
                                      "or link target not accessible: %s",
                                      r->filename);
                        return r->status = res;
                    }
                }
            }
            return OK;
        }

        if (cache->walked->nelts) {
            now_merged = ((walk_walked_t*)cache->walked->elts)
                [cache->walked->nelts - 1].merged;
        }
    }
    else {
        /* We start now_merged from NULL since we want to build
         * a locations list that can be merged to any vhost.
         */
        int sec_idx;
        int matches = cache->walked->nelts;
        int cached_matches = matches;
        walk_walked_t *last_walk = (walk_walked_t*)cache->walked->elts;
        core_dir_config *this_dir;
        core_opts_t opts;
        apr_finfo_t thisinfo;
        char *save_path_info;
        apr_size_t buflen;
        char *buf;
        unsigned int seg, startseg;
        apr_pool_t *rxpool = NULL;

        /* Invariant: from the first time filename_len is set until
         * it goes out of scope, filename_len==strlen(r->filename)
         */
        apr_size_t filename_len;
#ifdef CASE_BLIND_FILESYSTEM
        apr_size_t canonical_len;
#endif

        cached &= auth_internal_per_conf;

        /*
         * We must play our own mini-merge game here, for the few
         * running dir_config values we care about within dir_walk.
         * We didn't start the merge from r->per_dir_config, so we
         * accumulate opts and override as we merge, from the globals.
         */
        this_dir = ap_get_core_module_config(r->per_dir_config);
        opts.opts = this_dir->opts;
        opts.add = this_dir->opts_add;
        opts.remove = this_dir->opts_remove;
        opts.override = this_dir->override;
        opts.override_opts = this_dir->override_opts;
        opts.override_list = this_dir->override_list;

        /* Set aside path_info to merge back onto path_info later.
         * If r->filename is a directory, we must remerge the path_info,
         * before we continue!  [Directories cannot, by definition, have
         * path info.  Either the next segment is not-found, or a file.]
         *
         * r->path_info tracks the unconsumed source path.
         * r->filename  tracks the path as we process it
         */
        if ((r->finfo.filetype == APR_DIR) && r->path_info && *r->path_info)
        {
            if ((rv = apr_filepath_merge(&r->path_info, r->filename,
                                         r->path_info,
                                         APR_FILEPATH_NOTABOVEROOT, r->pool))
                != APR_SUCCESS) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00033)
                              "dir_walk error, path_info %s is not relative "
                              "to the filename path %s for uri %s",
                              r->path_info, r->filename, r->uri);
                return HTTP_INTERNAL_SERVER_ERROR;
            }

            save_path_info = NULL;
        }
        else {
            save_path_info = r->path_info;
            r->path_info = r->filename;
        }

#ifdef CASE_BLIND_FILESYSTEM

        canonical_len = 0;
        while (r->canonical_filename && r->canonical_filename[canonical_len]
               && (r->canonical_filename[canonical_len]
                   == r->path_info[canonical_len])) {
             ++canonical_len;
        }

        while (canonical_len
               && ((r->canonical_filename[canonical_len - 1] != '/'
                   && r->canonical_filename[canonical_len - 1])
                   || (r->path_info[canonical_len - 1] != '/'
                       && r->path_info[canonical_len - 1]))) {
            --canonical_len;
        }

        /*
         * Now build r->filename component by component, starting
         * with the root (on Unix, simply "/").  We will make a huge
         * assumption here for efficiency, that any canonical path
         * already given included a canonical root.
         */
        rv = apr_filepath_root((const char **)&r->filename,
                               (const char **)&r->path_info,
                               canonical_len ? 0 : APR_FILEPATH_TRUENAME,
                               r->pool);
        filename_len = strlen(r->filename);

        /*
         * Bad assumption above?  If the root's length is longer
         * than the canonical length, then it cannot be trusted as
         * a truename.  So try again, this time more seriously.
         */
        if ((rv == APR_SUCCESS) && canonical_len
            && (filename_len > canonical_len)) {
            rv = apr_filepath_root((const char **)&r->filename,
                                   (const char **)&r->path_info,
                                   APR_FILEPATH_TRUENAME, r->pool);
            filename_len = strlen(r->filename);
            canonical_len = 0;
        }

#else /* ndef CASE_BLIND_FILESYSTEM, really this simple for Unix today; */

        rv = apr_filepath_root((const char **)&r->filename,
                               (const char **)&r->path_info,
                               0, r->pool);
        filename_len = strlen(r->filename);

#endif

        if (rv != APR_SUCCESS) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00034)
                          "dir_walk error, could not determine the root "
                          "path of filename %s%s for uri %s",
                          r->filename, r->path_info, r->uri);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        /* Working space for terminating null and an extra / is required.
         */
        buflen = filename_len + strlen(r->path_info) + 2;
        buf = apr_palloc(r->pool, buflen);
        memcpy(buf, r->filename, filename_len + 1);
        r->filename = buf;
        thisinfo.valid = APR_FINFO_TYPE;
        thisinfo.filetype = APR_DIR; /* It's the root, of course it's a dir */

        /*
         * seg keeps track of which segment we've copied.
         * sec_idx keeps track of which section we're on, since sections are
         *     ordered by number of segments. See core_reorder_directories
         * startseg tells us how many segments describe the root path
         *     e.g. the complete path "//host/foo/" to a UNC share (4)
         */
        startseg = seg = ap_count_dirs(r->filename);
        sec_idx = 0;

        /*
         * Go down the directory hierarchy.  Where we have to check for
         * symlinks, do so.  Where a .htaccess file has permission to
         * override anything, try to find one.
         */
        do {
            int res;
            char *seg_name;
            char *delim;
            int temp_slash=0;

            /* We have no trailing slash, but we sure would appreciate one.
             * However, we don't want to append a / our first time through.
             */
            if ((seg > startseg) && r->filename[filename_len-1] != '/') {
                r->filename[filename_len++] = '/';
                r->filename[filename_len] = 0;
                temp_slash=1;
            }

            /* Begin *this* level by looking for matching <Directory> sections
             * from the server config.
             */
            for (; sec_idx < num_sec; ++sec_idx) {

                ap_conf_vector_t *entry_config = sec_ent[sec_idx];
                core_dir_config *entry_core;
                entry_core = ap_get_core_module_config(entry_config);

                /* No more possible matches for this many segments?
                 * We are done when we find relative/regex/longer components.
                 */
                if (entry_core->r || entry_core->d_components > seg) {
                    break;
                }

                /* We will never skip '0' element components, e.g. plain old
                 * <Directory >, and <Directory "/"> are classified as zero
                 * so that Win32/Netware/OS2 etc all pick them up.
                 * Otherwise, skip over the mismatches.
                 */
                if (entry_core->d_components
                    && ((entry_core->d_components < seg)
                     || (entry_core->d_is_fnmatch
                         ? (apr_fnmatch(entry_core->d, r->filename,
                                        APR_FNM_PATHNAME) != APR_SUCCESS)
                         : (strcmp(r->filename, entry_core->d) != 0)))) {
                    continue;
                }

                /* If we haven't continue'd above, we have a match.
                 *
                 * Calculate our full-context core opts & override.
                 */
                core_opts_merge(sec_ent[sec_idx], &opts);

                /* If we merged this same section last time, reuse it
                 */
                if (matches) {
                    if (last_walk->matched == sec_ent[sec_idx]) {
                        now_merged = last_walk->merged;
                        ++last_walk;
                        --matches;
                        continue;
                    }

                    /* We fell out of sync.  This is our own copy of walked,
                     * so truncate the remaining matches and reset remaining.
                     */
                    cache->walked->nelts -= matches;
                    matches = 0;
                    cached = 0;
                }

                if (now_merged) {
                    now_merged = ap_merge_per_dir_configs(r->pool,
                                                          now_merged,
                                                          sec_ent[sec_idx]);
                }
                else {
                    now_merged = sec_ent[sec_idx];
                }

                last_walk = (walk_walked_t*)apr_array_push(cache->walked);
                last_walk->matched = sec_ent[sec_idx];
                last_walk->merged = now_merged;
            }

            /* If .htaccess files are enabled, check for one, provided we
             * have reached a real path.
             */
            do {  /* Not really a loop, just a break'able code block */

                ap_conf_vector_t *htaccess_conf = NULL;

                /* No htaccess in an incomplete root path,
                 * nor if it's disabled
                 */
                if (seg < startseg || (!opts.override && opts.override_list == NULL)) {
                    break;
                }


                res = ap_parse_htaccess(&htaccess_conf, r, opts.override,
                                        opts.override_opts, opts.override_list,
                                        apr_pstrdup(r->pool, r->filename),
                                        sconf->access_name);
                if (res) {
                    return res;
                }

                if (!htaccess_conf) {
                    break;
                }

                /* If we are still here, we found our htaccess.
                 *
                 * Calculate our full-context core opts & override.
                 */
                core_opts_merge(htaccess_conf, &opts);

                /* If we merged this same htaccess last time, reuse it...
                 * this wouldn't work except that we cache the htaccess
                 * sections for the lifetime of the request, so we match
                 * the same conf.  Good planning (no, pure luck ;)
                 */
                if (matches) {
                    if (last_walk->matched == htaccess_conf) {
                        now_merged = last_walk->merged;
                        ++last_walk;
                        --matches;
                        break;
                    }

                    /* We fell out of sync.  This is our own copy of walked,
                     * so truncate the remaining matches and reset
                     * remaining.
                     */
                    cache->walked->nelts -= matches;
                    matches = 0;
                    cached = 0;
                }

                if (now_merged) {
                    now_merged = ap_merge_per_dir_configs(r->pool,
                                                          now_merged,
                                                          htaccess_conf);
                }
                else {
                    now_merged = htaccess_conf;
                }

                last_walk = (walk_walked_t*)apr_array_push(cache->walked);
                last_walk->matched = htaccess_conf;
                last_walk->merged = now_merged;

            } while (0); /* Only one htaccess, not a real loop */

            /* That temporary trailing slash was useful, now drop it.
             */
            if (temp_slash) {
                r->filename[--filename_len] = '\0';
            }

            /* Time for all good things to come to an end?
             */
            if (!r->path_info || !*r->path_info) {
                break;
            }

            /* Now it's time for the next segment...
             * We will assume the next element is an end node, and fix it up
             * below as necessary...
             */

            seg_name = r->filename + filename_len;
            delim = strchr(r->path_info + (*r->path_info == '/' ? 1 : 0), '/');
            if (delim) {
                apr_size_t path_info_len = delim - r->path_info;
                *delim = '\0';
                memcpy(seg_name, r->path_info, path_info_len + 1);
                filename_len += path_info_len;
                r->path_info = delim;
                *delim = '/';
            }
            else {
                apr_size_t path_info_len = strlen(r->path_info);
                memcpy(seg_name, r->path_info, path_info_len + 1);
                filename_len += path_info_len;
                r->path_info += path_info_len;
            }
            if (*seg_name == '/')
                ++seg_name;

            /* If nothing remained but a '/' string, we are finished
             * XXX: NO WE ARE NOT!!!  Now process this puppy!!! */
            if (!*seg_name) {
                break;
            }

            /* First optimization;
             * If...we knew r->filename was a file, and
             * if...we have strict (case-sensitive) filenames, or
             *      we know the canonical_filename matches to _this_ name, and
             * if...we have allowed symlinks
             * skip the lstat and dummy up an APR_DIR value for thisinfo.
             */
            if (r->finfo.filetype != APR_NOFILE
#ifdef CASE_BLIND_FILESYSTEM
                && (filename_len <= canonical_len)
#endif
                && ((opts.opts & (OPT_SYM_OWNER | OPT_SYM_LINKS)) == OPT_SYM_LINKS))
            {

                thisinfo.filetype = APR_DIR;
                ++seg;
                continue;
            }

            /* We choose apr_stat with flag APR_FINFO_LINK here, rather that
             * plain apr_stat, so that we capture this path object rather than
             * its target.  We will replace the info with our target's info
             * below.  We especially want the name of this 'link' object, not
             * the name of its target, if we are fixing the filename
             * case/resolving aliases.
             */
            rv = ap_run_dirwalk_stat(&thisinfo, r,
                                     APR_FINFO_MIN | APR_FINFO_NAME | APR_FINFO_LINK);

            if (APR_STATUS_IS_ENOENT(rv)) {
                /* Nothing?  That could be nice.  But our directory
                 * walk is done.
                 */
                thisinfo.filetype = APR_NOFILE;
                break;
            }
            else if (APR_STATUS_IS_EACCES(rv)) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00035)
                              "access to %s denied (filesystem path '%s') "
                              "because search permissions are missing on a "
                              "component of the path", r->uri, r->filename);
                return r->status = HTTP_FORBIDDEN;
            }
            else if ((rv != APR_SUCCESS && rv != APR_INCOMPLETE)
                     || !(thisinfo.valid & APR_FINFO_TYPE)) {
                /* If we hit ENOTDIR, we must have over-optimized, deny
                 * rather than assume not found.
                 */
                ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00036)
                              "access to %s failed (filesystem path '%s')", 
                              r->uri, r->filename);
                return r->status = HTTP_FORBIDDEN;
            }

            /* Fix up the path now if we have a name, and they don't agree
             */
            if ((thisinfo.valid & APR_FINFO_NAME)
                && strcmp(seg_name, thisinfo.name)) {
                /* TODO: provide users an option that an internal/external
                 * redirect is required here?  We need to walk the URI and
                 * filename in tandem to properly correlate these.
                 */
                strcpy(seg_name, thisinfo.name);
                filename_len = strlen(r->filename);
            }

            if (thisinfo.filetype == APR_LNK) {
                /* Is this a possibly acceptable symlink?
                 */
                if ((res = resolve_symlink(r->filename, &thisinfo,
                                           opts.opts, r->pool)) != OK) {
                    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00037)
                                  "Symbolic link not allowed "
                                  "or link target not accessible: %s",
                                  r->filename);
                    return r->status = res;
                }
            }

            /* Ok, we are done with the link's info, test the real target
             */
            if (thisinfo.filetype == APR_REG ||
                thisinfo.filetype == APR_NOFILE) {
                /* That was fun, nothing left for us here
                 */
                break;
            }
            else if (thisinfo.filetype != APR_DIR) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00038)
                              "Forbidden: %s doesn't point to "
                              "a file or directory",
                              r->filename);
                return r->status = HTTP_FORBIDDEN;
            }

            ++seg;
        } while (thisinfo.filetype == APR_DIR);

        /* If we have _not_ optimized, this is the time to recover
         * the final stat result.
         */
        if (r->finfo.filetype == APR_NOFILE || r->finfo.filetype == APR_LNK) {
            r->finfo = thisinfo;
        }

        /* Now splice the saved path_info back onto any new path_info
         */
        if (save_path_info) {
            if (r->path_info && *r->path_info) {
                r->path_info = ap_make_full_path(r->pool, r->path_info,
                                                 save_path_info);
            }
            else {
                r->path_info = save_path_info;
            }
        }

        /*
         * Now we'll deal with the regexes, note we pick up sec_idx
         * where we left off (we gave up after we hit entry_core->r)
         */
        for (; sec_idx < num_sec; ++sec_idx) {

            int nmatch = 0;
            int i;
            ap_regmatch_t *pmatch = NULL;

            core_dir_config *entry_core;
            entry_core = ap_get_core_module_config(sec_ent[sec_idx]);

            if (!entry_core->r) {
                continue;
            }

            if (entry_core->refs && entry_core->refs->nelts) {
                if (!rxpool) {
                    apr_pool_create(&rxpool, r->pool);
                }
                nmatch = entry_core->refs->nelts;
                pmatch = apr_palloc(rxpool, nmatch*sizeof(ap_regmatch_t));
            }

            if (ap_regexec(entry_core->r, r->filename, nmatch, pmatch, 0)) {
                continue;
            }

            for (i = 0; i < nmatch; i++) {
                if (pmatch[i].rm_so >= 0 && pmatch[i].rm_eo >= 0 &&
                    ((const char **)entry_core->refs->elts)[i]) {
                    apr_table_setn(r->subprocess_env, 
                                   ((const char **)entry_core->refs->elts)[i],
                                   apr_pstrndup(r->pool,
                                   r->filename + pmatch[i].rm_so,
                                   pmatch[i].rm_eo - pmatch[i].rm_so));
                }
            }

            /* If we haven't already continue'd above, we have a match.
             *
             * Calculate our full-context core opts & override.
             */
            core_opts_merge(sec_ent[sec_idx], &opts);

            /* If we merged this same section last time, reuse it
             */
            if (matches) {
                if (last_walk->matched == sec_ent[sec_idx]) {
                    now_merged = last_walk->merged;
                    ++last_walk;
                    --matches;
                    continue;
                }

                /* We fell out of sync.  This is our own copy of walked,
                 * so truncate the remaining matches and reset remaining.
                 */
                cache->walked->nelts -= matches;
                matches = 0;
                cached = 0;
            }

            if (now_merged) {
                now_merged = ap_merge_per_dir_configs(r->pool,
                                                      now_merged,
                                                      sec_ent[sec_idx]);
            }
            else {
                now_merged = sec_ent[sec_idx];
            }

            last_walk = (walk_walked_t*)apr_array_push(cache->walked);
            last_walk->matched = sec_ent[sec_idx];
            last_walk->merged = now_merged;
        }

        if (rxpool) {
            apr_pool_destroy(rxpool);
        }

        /* Whoops - everything matched in sequence, but either the original
         * walk found some additional matches (which we need to truncate), or
         * this walk found some additional matches.
         */
        if (matches) {
            cache->walked->nelts -= matches;
            cached = 0;
        }
        else if (cache->walked->nelts > cached_matches) {
            cached = 0;
        }
    }

/* It seems this shouldn't be needed anymore.  We translated the
 x symlink above into a real resource, and should have died up there.
 x Even if we keep this, it needs more thought (maybe an r->file_is_symlink)
 x perhaps it should actually happen in file_walk, so we catch more
 x obscure cases in autoindex subrequests, etc.
 x
 x    * Symlink permissions are determined by the parent.  If the request is
 x    * for a directory then applying the symlink test here would use the
 x    * permissions of the directory as opposed to its parent.  Consider a
 x    * symlink pointing to a dir with a .htaccess disallowing symlinks.  If
 x    * you access /symlink (or /symlink/) you would get a 403 without this
 x    * APR_DIR test.  But if you accessed /symlink/index.html, for example,
 x    * you would *not* get the 403.
 x
 x   if (r->finfo.filetype != APR_DIR
 x       && (res = resolve_symlink(r->filename, r->info, ap_allow_options(r),
 x                                 r->pool))) {
 x       ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
 x                     "Symbolic link not allowed: %s", r->filename);
 x       return res;
 x   }
 */

    /* Save future sub-requestors much angst in processing
     * this subrequest.  If dir_walk couldn't canonicalize
     * the file path, nothing can.
     */
    r->canonical_filename = r->filename;

    if (r->finfo.filetype == APR_DIR) {
        cache->cached = r->filename;
    }
    else {
        cache->cached = ap_make_dirstr_parent(r->pool, r->filename);
    }

    if (cached
        && r->per_dir_config == cache->dir_conf_merged) {
        r->per_dir_config = cache->per_dir_result;
        return OK;
    }

    cache->dir_conf_tested = sec_ent;
    cache->dir_conf_merged = r->per_dir_config;

    /* Merge our cache->dir_conf_merged construct with the r->per_dir_configs,
     * and note the end result to (potentially) skip this step next time.
     */
    if (now_merged) {
        r->per_dir_config = ap_merge_per_dir_configs(r->pool,
                                                     r->per_dir_config,
                                                     now_merged);
    }
    cache->per_dir_result = r->per_dir_config;

    return OK;
}