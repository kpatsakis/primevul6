crm_client_new(qb_ipcs_connection_t * c, uid_t uid, gid_t gid)
{
    static gid_t crm_gid = 0;
    crm_client_t *client = NULL;

    CRM_LOG_ASSERT(c);
    if (c == NULL) {
        return NULL;
    }

    if (crm_gid == 0 && crm_user_lookup(CRM_DAEMON_USER, NULL, &crm_gid) < 0) {
        static bool have_error = FALSE;
        if(have_error == FALSE) {
            crm_warn("Could not find group for user %s", CRM_DAEMON_USER);
            have_error = TRUE;
        }
    }

    if(crm_gid != 0 && gid != 0) {
        crm_trace("Giving access to group %u", crm_gid);
        qb_ipcs_connection_auth_set(c, -1, crm_gid, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    }

    crm_client_init();

    client = calloc(1, sizeof(crm_client_t));

    client->ipcs = c;
    client->kind = CRM_CLIENT_IPC;
    client->pid = crm_ipcs_client_pid(c);

    client->id = crm_generate_uuid();

    crm_info("Connecting %p for uid=%d gid=%d pid=%u id=%s", c, uid, gid, client->pid, client->id);

#if ENABLE_ACL
    client->user = uid2username(uid);
#endif

    g_hash_table_insert(client_connections, c, client);
    return client;
}