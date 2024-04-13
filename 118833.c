irc_server_outqueue_free (struct t_irc_server *server,
                          int priority,
                          struct t_irc_outqueue *outqueue)
{
    struct t_irc_outqueue *new_outqueue;

    if (!server || !outqueue)
        return;

    /* remove outqueue message */
    if (server->last_outqueue[priority] == outqueue)
        server->last_outqueue[priority] = outqueue->prev_outqueue;
    if (outqueue->prev_outqueue)
    {
        (outqueue->prev_outqueue)->next_outqueue = outqueue->next_outqueue;
        new_outqueue = server->outqueue[priority];
    }
    else
        new_outqueue = outqueue->next_outqueue;

    if (outqueue->next_outqueue)
        (outqueue->next_outqueue)->prev_outqueue = outqueue->prev_outqueue;

    /* free data */
    if (outqueue->command)
        free (outqueue->command);
    if (outqueue->message_before_mod)
        free (outqueue->message_before_mod);
    if (outqueue->message_after_mod)
        free (outqueue->message_after_mod);
    if (outqueue->tags)
        free (outqueue->tags);
    free (outqueue);

    /* set new head */
    server->outqueue[priority] = new_outqueue;
}