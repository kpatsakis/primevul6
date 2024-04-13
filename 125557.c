void streamPropagateConsumerCreation(client *c, robj *key, robj *groupname, sds consumername) {
    robj *argv[5];
    argv[0] = shared.xgroup;
    argv[1] = shared.createconsumer;
    argv[2] = key;
    argv[3] = groupname;
    argv[4] = createObject(OBJ_STRING,sdsdup(consumername));

    alsoPropagate(c->db->id,argv,5,PROPAGATE_AOF|PROPAGATE_REPL);

    decrRefCount(argv[4]);
}