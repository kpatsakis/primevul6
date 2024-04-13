AP_DECLARE(int) ap_file_walk(request_rec *r)
{
    ap_conf_vector_t *now_merged = NULL;
    core_dir_config *dconf = ap_get_core_module_config(r->per_dir_config);
    ap_conf_vector_t **sec_ent = NULL;
    int num_sec = 0;
    walk_cache_t *cache;
    const char *test_file;
    int cached;

    if (dconf->sec_file) {
        sec_ent = (ap_conf_vector_t **)dconf->sec_file->elts;
        num_sec = dconf->sec_file->nelts;
    }

    /* To allow broken modules to proceed, we allow missing filenames to pass.
     * We will catch it later if it's heading for the core handler.
     * directory_walk already posted an INFO note for module debugging.
     */
    if (r->filename == NULL) {
        return OK;
    }

    cache = prep_walk_cache(AP_NOTE_FILE_WALK, r);
    cached = (cache->cached != NULL);

    /* No tricks here, there are just no <Files > to parse in this context.
     * We won't destroy the cache, just in case _this_ redirect is later
     * redirected again to a context containing the same or similar <Files >.
     */
    if (!num_sec) {
        return OK;
    }

    /* Get the basename .. and copy for the cache just
     * in case r->filename is munged by another module
     */
    test_file = strrchr(r->filename, '/');
    if (test_file == NULL) {
        test_file = apr_pstrdup(r->pool, r->filename);
    }
    else {
        test_file = apr_pstrdup(r->pool, ++test_file);
    }

    /* If we have an cache->cached file name that matches test_file,
     * and the directory's list of file sections hasn't changed, we
     * can skip rewalking the file_walk entries.
     */
    if (cached
        && (cache->dir_conf_tested == sec_ent)
        && (strcmp(test_file, cache->cached) == 0)) {
        /* Well this looks really familiar!  If our end-result (per_dir_result)
         * didn't change, we have absolutely nothing to do :)
         * Otherwise (as is the case with most dir_merged requests)
         * we must merge our dir_conf_merged onto this new r->per_dir_config.
         */
        if (r->per_dir_config == cache->per_dir_result) {
            return OK;
        }

        if (cache->walked->nelts) {
            now_merged = ((walk_walked_t*)cache->walked->elts)
                [cache->walked->nelts - 1].merged;
        }
    }
    else {
        /* We start now_merged from NULL since we want to build
         * a file section list that can be merged to any dir_walk.
         */
        int sec_idx;
        int matches = cache->walked->nelts;
        int cached_matches = matches;
        walk_walked_t *last_walk = (walk_walked_t*)cache->walked->elts;
        apr_pool_t *rxpool = NULL;

        cached &= auth_internal_per_conf;
        cache->cached = test_file;

        /* Go through the location entries, and check for matches.
         * We apply the directive sections in given order, we should
         * really try them with the most general first.
         */
        for (sec_idx = 0; sec_idx < num_sec; ++sec_idx) {
            core_dir_config *entry_core;
            entry_core = ap_get_core_module_config(sec_ent[sec_idx]);

            if (entry_core->r) {

                int nmatch = 0;
                int i;
                ap_regmatch_t *pmatch = NULL;

                if (entry_core->refs && entry_core->refs->nelts) {
                    if (!rxpool) {
                        apr_pool_create(&rxpool, r->pool);
                    }
                    nmatch = entry_core->refs->nelts;
                    pmatch = apr_palloc(rxpool, nmatch*sizeof(ap_regmatch_t));
                }

                if (ap_regexec(entry_core->r, cache->cached, nmatch, pmatch, 0)) {
                    continue;
                }

                for (i = 0; i < nmatch; i++) {
                    if (pmatch[i].rm_so >= 0 && pmatch[i].rm_eo >= 0 && 
                        ((const char **)entry_core->refs->elts)[i]) {
                        apr_table_setn(r->subprocess_env,
                                       ((const char **)entry_core->refs->elts)[i],
                                       apr_pstrndup(r->pool,
                                       cache->cached + pmatch[i].rm_so,
                                       pmatch[i].rm_eo - pmatch[i].rm_so));
                    }
                }

            }
            else {
                if ((entry_core->d_is_fnmatch
                       ? apr_fnmatch(entry_core->d, cache->cached, APR_FNM_PATHNAME)
                       : strcmp(entry_core->d, cache->cached))) {
                    continue;
                }
            }

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