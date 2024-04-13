
static ssize_t bql_show_hold_time(struct netdev_queue *queue,
				  char *buf)
{
	struct dql *dql = &queue->dql;

	return sprintf(buf, "%u\n", jiffies_to_msecs(dql->slack_hold_time));