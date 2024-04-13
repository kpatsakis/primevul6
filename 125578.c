void xgroupCommand(client *c) {
    stream *s = NULL;
    sds grpname = NULL;
    streamCG *cg = NULL;
    char *opt = c->argv[1]->ptr; /* Subcommand name. */
    int mkstream = 0;
    long long entries_read = SCG_INVALID_ENTRIES_READ;
    robj *o;

    /* Everything but the "HELP" option requires a key and group name. */
    if (c->argc >= 4) {
        /* Parse optional arguments for CREATE and SETID */
        int i = 5;
        int create_subcmd = !strcasecmp(opt,"CREATE");
        int setid_subcmd = !strcasecmp(opt,"SETID");
        while (i < c->argc) {
            if (create_subcmd && !strcasecmp(c->argv[i]->ptr,"MKSTREAM")) {
                mkstream = 1;
                i++;
            } else if ((create_subcmd || setid_subcmd) && !strcasecmp(c->argv[i]->ptr,"ENTRIESREAD") && i + 1 < c->argc) {
                if (getLongLongFromObjectOrReply(c,c->argv[i+1],&entries_read,NULL) != C_OK)
                    return;
                if (entries_read < 0 && entries_read != SCG_INVALID_ENTRIES_READ) {
                    addReplyError(c,"value for ENTRIESREAD must be positive or -1");
                    return;
                }
                i += 2;
            } else {
                addReplySubcommandSyntaxError(c);
                return;
            }
        }

        o = lookupKeyWrite(c->db,c->argv[2]);
        if (o) {
            if (checkType(c,o,OBJ_STREAM)) return;
            s = o->ptr;
        }
        grpname = c->argv[3]->ptr;
    }

    /* Check for missing key/group. */
    if (c->argc >= 4 && !mkstream) {
        /* At this point key must exist, or there is an error. */
        if (s == NULL) {
            addReplyError(c,
                "The XGROUP subcommand requires the key to exist. "
                "Note that for CREATE you may want to use the MKSTREAM "
                "option to create an empty stream automatically.");
            return;
        }

        /* Certain subcommands require the group to exist. */
        if ((cg = streamLookupCG(s,grpname)) == NULL &&
            (!strcasecmp(opt,"SETID") ||
             !strcasecmp(opt,"CREATECONSUMER") ||
             !strcasecmp(opt,"DELCONSUMER")))
        {
            addReplyErrorFormat(c, "-NOGROUP No such consumer group '%s' "
                                   "for key name '%s'",
                                   (char*)grpname, (char*)c->argv[2]->ptr);
            return;
        }
    }

    /* Dispatch the different subcommands. */
    if (c->argc == 2 && !strcasecmp(opt,"HELP")) {
        const char *help[] = {
"CREATE <key> <groupname> <id|$> [option]",
"    Create a new consumer group. Options are:",
"    * MKSTREAM",
"      Create the empty stream if it does not exist.",
"    * ENTRIESREAD entries_read",
"      Set the group's entries_read counter (internal use).",
"CREATECONSUMER <key> <groupname> <consumer>",
"    Create a new consumer in the specified group.",
"DELCONSUMER <key> <groupname> <consumer>",
"    Remove the specified consumer.",
"DESTROY <key> <groupname>",
"    Remove the specified group.",
"SETID <key> <groupname> <id|$> [ENTRIESREAD entries_read]",
"    Set the current group ID and entries_read counter.",
NULL
        };
        addReplyHelp(c, help);
    } else if (!strcasecmp(opt,"CREATE") && (c->argc >= 5 && c->argc <= 8)) {
        streamID id;
        if (!strcmp(c->argv[4]->ptr,"$")) {
            if (s) {
                id = s->last_id;
            } else {
                id.ms = 0;
                id.seq = 0;
            }
        } else if (streamParseStrictIDOrReply(c,c->argv[4],&id,0,NULL) != C_OK) {
            return;
        }

        /* Handle the MKSTREAM option now that the command can no longer fail. */
        if (s == NULL) {
            serverAssert(mkstream);
            o = createStreamObject();
            dbAdd(c->db,c->argv[2],o);
            s = o->ptr;
            signalModifiedKey(c,c->db,c->argv[2]);
        }

        streamCG *cg = streamCreateCG(s,grpname,sdslen(grpname),&id,entries_read);
        if (cg) {
            addReply(c,shared.ok);
            server.dirty++;
            notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-create",
                                c->argv[2],c->db->id);
        } else {
            addReplyError(c,"-BUSYGROUP Consumer Group name already exists");
        }
    } else if (!strcasecmp(opt,"SETID") && (c->argc == 5 || c->argc == 7)) {
        streamID id;
        if (!strcmp(c->argv[4]->ptr,"$")) {
            id = s->last_id;
        } else if (streamParseIDOrReply(c,c->argv[4],&id,0) != C_OK) {
            return;
        }
        cg->last_id = id;
        cg->entries_read = entries_read;
        addReply(c,shared.ok);
        server.dirty++;
        notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-setid",c->argv[2],c->db->id);
    } else if (!strcasecmp(opt,"DESTROY") && c->argc == 4) {
        if (cg) {
            raxRemove(s->cgroups,(unsigned char*)grpname,sdslen(grpname),NULL);
            streamFreeCG(cg);
            addReply(c,shared.cone);
            server.dirty++;
            notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-destroy",
                                c->argv[2],c->db->id);
            /* We want to unblock any XREADGROUP consumers with -NOGROUP. */
            signalKeyAsReady(c->db,c->argv[2],OBJ_STREAM);
        } else {
            addReply(c,shared.czero);
        }
    } else if (!strcasecmp(opt,"CREATECONSUMER") && c->argc == 5) {
        streamConsumer *created = streamCreateConsumer(cg,c->argv[4]->ptr,c->argv[2],
                                                       c->db->id,SCC_DEFAULT);
        addReplyLongLong(c,created ? 1 : 0);
    } else if (!strcasecmp(opt,"DELCONSUMER") && c->argc == 5) {
        long long pending = 0;
        streamConsumer *consumer = streamLookupConsumer(cg,c->argv[4]->ptr,SLC_NO_REFRESH);
        if (consumer) {
            /* Delete the consumer and returns the number of pending messages
             * that were yet associated with such a consumer. */
            pending = raxSize(consumer->pel);
            streamDelConsumer(cg,consumer);
            server.dirty++;
            notifyKeyspaceEvent(NOTIFY_STREAM,"xgroup-delconsumer",
                                c->argv[2],c->db->id);
        }
        addReplyLongLong(c,pending);
    } else {
        addReplySubcommandSyntaxError(c);
    }
}