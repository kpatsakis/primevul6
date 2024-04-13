static int streamParseAddOrTrimArgsOrReply(client *c, streamAddTrimArgs *args, int xadd) {
    /* Initialize arguments to defaults */
    memset(args, 0, sizeof(*args));

    /* Parse options. */
    int i = 2; /* This is the first argument position where we could
                  find an option, or the ID. */
    int limit_given = 0;
    for (; i < c->argc; i++) {
        int moreargs = (c->argc-1) - i; /* Number of additional arguments. */
        char *opt = c->argv[i]->ptr;
        if (xadd && opt[0] == '*' && opt[1] == '\0') {
            /* This is just a fast path for the common case of auto-ID
             * creation. */
            break;
        } else if (!strcasecmp(opt,"maxlen") && moreargs) {
            if (args->trim_strategy != TRIM_STRATEGY_NONE) {
                addReplyError(c,"syntax error, MAXLEN and MINID options at the same time are not compatible");
                return -1;
            }
            args->approx_trim = 0;
            char *next = c->argv[i+1]->ptr;
            /* Check for the form MAXLEN ~ <count>. */
            if (moreargs >= 2 && next[0] == '~' && next[1] == '\0') {
                args->approx_trim = 1;
                i++;
            } else if (moreargs >= 2 && next[0] == '=' && next[1] == '\0') {
                i++;
            }
            if (getLongLongFromObjectOrReply(c,c->argv[i+1],&args->maxlen,NULL)
                != C_OK) return -1;

            if (args->maxlen < 0) {
                addReplyError(c,"The MAXLEN argument must be >= 0.");
                return -1;
            }
            i++;
            args->trim_strategy = TRIM_STRATEGY_MAXLEN;
            args->trim_strategy_arg_idx = i;
        } else if (!strcasecmp(opt,"minid") && moreargs) {
            if (args->trim_strategy != TRIM_STRATEGY_NONE) {
                addReplyError(c,"syntax error, MAXLEN and MINID options at the same time are not compatible");
                return -1;
            }
            args->approx_trim = 0;
            char *next = c->argv[i+1]->ptr;
            /* Check for the form MINID ~ <id>|<age>. */
            if (moreargs >= 2 && next[0] == '~' && next[1] == '\0') {
                args->approx_trim = 1;
                i++;
            } else if (moreargs >= 2 && next[0] == '=' && next[1] == '\0') {
                i++;
            }

            if (streamParseStrictIDOrReply(c,c->argv[i+1],&args->minid,0,NULL) != C_OK)
                return -1;
            
            i++;
            args->trim_strategy = TRIM_STRATEGY_MINID;
            args->trim_strategy_arg_idx = i;
        } else if (!strcasecmp(opt,"limit") && moreargs) {
            /* Note about LIMIT: If it was not provided by the caller we set
             * it to 100*server.stream_node_max_entries, and that's to prevent the
             * trimming from taking too long, on the expense of not deleting entries
             * that should be trimmed.
             * If user wanted exact trimming (i.e. no '~') we never limit the number
             * of trimmed entries */
            if (getLongLongFromObjectOrReply(c,c->argv[i+1],&args->limit,NULL) != C_OK)
                return -1;

            if (args->limit < 0) {
                addReplyError(c,"The LIMIT argument must be >= 0.");
                return -1;
            }
            limit_given = 1;
            i++;
        } else if (xadd && !strcasecmp(opt,"nomkstream")) {
            args->no_mkstream = 1;
        } else if (xadd) {
            /* If we are here is a syntax error or a valid ID. */
            if (streamParseStrictIDOrReply(c,c->argv[i],&args->id,0,&args->seq_given) != C_OK)
                return -1;
            args->id_given = 1;
            break;
        } else {
            addReplyErrorObject(c,shared.syntaxerr);
            return -1;
        }
    }

    if (args->limit && args->trim_strategy == TRIM_STRATEGY_NONE) {
        addReplyError(c,"syntax error, LIMIT cannot be used without specifying a trimming strategy");
        return -1;
    }

    if (!xadd && args->trim_strategy == TRIM_STRATEGY_NONE) {
        addReplyError(c,"syntax error, XTRIM must be called with a trimming strategy");
        return -1;
    }

    if (mustObeyClient(c)) {
        /* If command came from master or from AOF we must not enforce maxnodes
         * (The maxlen/minid argument was re-written to make sure there's no
         * inconsistency). */
        args->limit = 0;
    } else {
        /* We need to set the limit (only if we got '~') */
        if (limit_given) {
            if (!args->approx_trim) {
                /* LIMIT was provided without ~ */
                addReplyError(c,"syntax error, LIMIT cannot be used without the special ~ option");
                return -1;
            }
        } else {
            /* User didn't provide LIMIT, we must set it. */
            if (args->approx_trim) {
                /* In order to prevent from trimming to do too much work and 
                 * cause latency spikes we limit the amount of work it can do.
                 * We have to cap args->limit from both sides in case 
                 * stream_node_max_entries is 0 or too big (could cause overflow)
                 */
                args->limit = 100 * server.stream_node_max_entries; /* Maximum 100 rax nodes. */
                if (args->limit <= 0) args->limit = 10000;
                if (args->limit > 1000000) args->limit = 1000000;
            } else {
                /* No LIMIT for exact trimming */
                args->limit = 0;
            }
        }
    }

    return i;
}