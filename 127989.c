
static ssize_t bql_set_hold_time(struct netdev_queue *queue,
				 const char *buf, size_t len)
{
	struct dql *dql = &queue->dql;
	unsigned int value;
	int err;

	err = kstrtouint(buf, 10, &value);
	if (err < 0)
		return err;

	dql->slack_hold_time = msecs_to_jiffies(value);

	return len;