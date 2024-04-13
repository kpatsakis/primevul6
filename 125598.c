void streamRewriteTrimArgument(client *c, stream *s, int trim_strategy, int idx) {
    robj *arg;
    if (trim_strategy == TRIM_STRATEGY_MAXLEN) {
        arg = createStringObjectFromLongLong(s->length);
    } else {
        streamID first_id;
        streamGetEdgeID(s,1,0,&first_id);
        arg = createObjectFromStreamID(&first_id);
    }

    rewriteClientCommandArgument(c,idx,arg);
    decrRefCount(arg);
}