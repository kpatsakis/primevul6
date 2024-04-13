irc_server_outqueue_add (struct t_irc_server *server, int priority,
                         const char *command, const char *msg1,
                         const char *msg2, int modified, const char *tags,
                         struct t_irc_redirect *redirect)
{
    struct t_irc_outqueue *new_outqueue;

    new_outqueue = malloc (sizeof (*new_outqueue));
    if (new_outqueue)
    {
        new_outqueue->command = (command) ? strdup (command) : strdup ("unknown");
        new_outqueue->message_before_mod = (msg1) ? strdup (msg1) : NULL;
        new_outqueue->message_after_mod = (msg2) ? strdup (msg2) : NULL;
        new_outqueue->modified = modified;
        new_outqueue->tags = (tags) ? strdup (tags) : NULL;
        new_outqueue->redirect = redirect;

        new_outqueue->prev_outqueue = server->last_outqueue[priority];
        new_outqueue->next_outqueue = NULL;
        if (server->last_outqueue[priority])
            server->last_outqueue[priority]->next_outqueue = new_outqueue;
        else
            server->outqueue[priority] = new_outqueue;
        server->last_outqueue[priority] = new_outqueue;
    }
}