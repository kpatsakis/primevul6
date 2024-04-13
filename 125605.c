void xtrimCommand(client *c) {
    robj *o;

    /* Argument parsing. */
    streamAddTrimArgs parsed_args;
    if (streamParseAddOrTrimArgsOrReply(c, &parsed_args, 0) < 0)
        return; /* streamParseAddOrTrimArgsOrReply already replied. */

    /* If the key does not exist, we are ok returning zero, that is, the
     * number of elements removed from the stream. */
    if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.czero)) == NULL
        || checkType(c,o,OBJ_STREAM)) return;
    stream *s = o->ptr;

    /* Perform the trimming. */
    int64_t deleted = streamTrim(s, &parsed_args);
    if (deleted) {
        notifyKeyspaceEvent(NOTIFY_STREAM,"xtrim",c->argv[1],c->db->id);
        if (parsed_args.approx_trim) {
            /* In case our trimming was limited (by LIMIT or by ~) we must
             * re-write the relevant trim argument to make sure there will be
             * no inconsistencies in AOF loading or in the replica.
             * It's enough to check only args->approx because there is no
             * way LIMIT is given without the ~ option. */
            streamRewriteApproxSpecifier(c,parsed_args.trim_strategy_arg_idx-1);
            streamRewriteTrimArgument(c,s,parsed_args.trim_strategy,parsed_args.trim_strategy_arg_idx);
        }

        /* Propagate the write. */
        signalModifiedKey(c, c->db,c->argv[1]);
        server.dirty += deleted;
    }
    addReplyLongLong(c,deleted);
}