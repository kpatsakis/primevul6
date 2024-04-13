ogs_pkbuf_t *ogs_sbi_find_part_by_content_id(
        ogs_sbi_message_t *message, char *content_id)
{
    int i;

    ogs_assert(message);
    ogs_assert(content_id);

    for (i = 0; i < message->num_of_part; i++) {
        if (message->part[i].content_id &&
            strcmp(message->part[i].content_id, content_id) == 0)
            return message->part[i].pkbuf;
    }

    return NULL;
}