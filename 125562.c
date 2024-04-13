robj *streamDup(robj *o) {
    robj *sobj;

    serverAssert(o->type == OBJ_STREAM);

    switch (o->encoding) {
        case OBJ_ENCODING_STREAM:
            sobj = createStreamObject();
            break;
        default:
            serverPanic("Wrong encoding.");
            break;
    }

    stream *s;
    stream *new_s;
    s = o->ptr;
    new_s = sobj->ptr;

    raxIterator ri;
    uint64_t rax_key[2];
    raxStart(&ri, s->rax);
    raxSeek(&ri, "^", NULL, 0);
    size_t lp_bytes = 0;      /* Total bytes in the listpack. */
    unsigned char *lp = NULL; /* listpack pointer. */
    /* Get a reference to the listpack node. */
    while (raxNext(&ri)) {
        lp = ri.data;
        lp_bytes = lpBytes(lp);
        unsigned char *new_lp = zmalloc(lp_bytes);
        memcpy(new_lp, lp, lp_bytes);
        memcpy(rax_key, ri.key, sizeof(rax_key));
        raxInsert(new_s->rax, (unsigned char *)&rax_key, sizeof(rax_key),
                  new_lp, NULL);
    }
    new_s->length = s->length;
    new_s->first_id = s->first_id;
    new_s->last_id = s->last_id;
    new_s->max_deleted_entry_id = s->max_deleted_entry_id;
    new_s->entries_added = s->entries_added;
    raxStop(&ri);

    if (s->cgroups == NULL) return sobj;

    /* Consumer Groups */
    raxIterator ri_cgroups;
    raxStart(&ri_cgroups, s->cgroups);
    raxSeek(&ri_cgroups, "^", NULL, 0);
    while (raxNext(&ri_cgroups)) {
        streamCG *cg = ri_cgroups.data;
        streamCG *new_cg = streamCreateCG(new_s, (char *)ri_cgroups.key,
                                          ri_cgroups.key_len, &cg->last_id,
                                          cg->entries_read);

        serverAssert(new_cg != NULL);

        /* Consumer Group PEL */
        raxIterator ri_cg_pel;
        raxStart(&ri_cg_pel,cg->pel);
        raxSeek(&ri_cg_pel,"^",NULL,0);
        while(raxNext(&ri_cg_pel)){
            streamNACK *nack = ri_cg_pel.data;
            streamNACK *new_nack = streamCreateNACK(NULL);
            new_nack->delivery_time = nack->delivery_time;
            new_nack->delivery_count = nack->delivery_count;
            raxInsert(new_cg->pel, ri_cg_pel.key, sizeof(streamID), new_nack, NULL);
        }
        raxStop(&ri_cg_pel);

        /* Consumers */
        raxIterator ri_consumers;
        raxStart(&ri_consumers, cg->consumers);
        raxSeek(&ri_consumers, "^", NULL, 0);
        while (raxNext(&ri_consumers)) {
            streamConsumer *consumer = ri_consumers.data;
            streamConsumer *new_consumer;
            new_consumer = zmalloc(sizeof(*new_consumer));
            new_consumer->name = sdsdup(consumer->name);
            new_consumer->pel = raxNew();
            raxInsert(new_cg->consumers,(unsigned char *)new_consumer->name,
                        sdslen(new_consumer->name), new_consumer, NULL);
            new_consumer->seen_time = consumer->seen_time;

            /* Consumer PEL */
            raxIterator ri_cpel;
            raxStart(&ri_cpel, consumer->pel);
            raxSeek(&ri_cpel, "^", NULL, 0);
            while (raxNext(&ri_cpel)) {
                streamNACK *new_nack = raxFind(new_cg->pel,ri_cpel.key,sizeof(streamID));

                serverAssert(new_nack != raxNotFound);

                new_nack->consumer = new_consumer;
                raxInsert(new_consumer->pel,ri_cpel.key,sizeof(streamID),new_nack,NULL);
            }
            raxStop(&ri_cpel);
        }
        raxStop(&ri_consumers);
    }
    raxStop(&ri_cgroups);
    return sobj;
}