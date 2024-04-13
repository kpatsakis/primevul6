
static ssize_t bql_show_inflight(struct netdev_queue *queue,
				 char *buf)
{
	struct dql *dql = &queue->dql;

	return sprintf(buf, "%u\n", dql->num_queued - dql->num_completed);