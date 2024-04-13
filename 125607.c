void streamPropagateXCLAIM(client *c, robj *key, streamCG *group, robj *groupname, robj *id, streamNACK *nack) {
    /* We need to generate an XCLAIM that will work in a idempotent fashion:
     *
     * XCLAIM <key> <group> <consumer> 0 <id> TIME <milliseconds-unix-time>
     *        RETRYCOUNT <count> FORCE JUSTID LASTID <id>.
     *
     * Note that JUSTID is useful in order to avoid that XCLAIM will do
     * useless work in the slave side, trying to fetch the stream item. */
    robj *argv[14];
    argv[0] = shared.xclaim;
    argv[1] = key;
    argv[2] = groupname;
    argv[3] = createStringObject(nack->consumer->name,sdslen(nack->consumer->name));
    argv[4] = shared.integers[0];
    argv[5] = id;
    argv[6] = shared.time;
    argv[7] = createStringObjectFromLongLong(nack->delivery_time);
    argv[8] = shared.retrycount;
    argv[9] = createStringObjectFromLongLong(nack->delivery_count);
    argv[10] = shared.force;
    argv[11] = shared.justid;
    argv[12] = shared.lastid;
    argv[13] = createObjectFromStreamID(&group->last_id);

    alsoPropagate(c->db->id,argv,14,PROPAGATE_AOF|PROPAGATE_REPL);

    decrRefCount(argv[3]);
    decrRefCount(argv[7]);
    decrRefCount(argv[9]);
    decrRefCount(argv[13]);
}