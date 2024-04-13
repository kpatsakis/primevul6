
static void rps_dev_flow_table_release(struct rcu_head *rcu)
{
	struct rps_dev_flow_table *table = container_of(rcu,
	    struct rps_dev_flow_table, rcu);
	vfree(table);