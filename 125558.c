void streamPropagateGroupID(client *c, robj *key, streamCG *group, robj *groupname) {
    robj *argv[7];
    argv[0] = shared.xgroup;
    argv[1] = shared.setid;
    argv[2] = key;
    argv[3] = groupname;
    argv[4] = createObjectFromStreamID(&group->last_id);
    argv[5] = shared.entriesread;
    argv[6] = createStringObjectFromLongLong(group->entries_read);

    alsoPropagate(c->db->id,argv,7,PROPAGATE_AOF|PROPAGATE_REPL);

    decrRefCount(argv[4]);
    decrRefCount(argv[6]);
}