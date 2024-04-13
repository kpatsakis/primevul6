static __init int smack_init(void)
{
	struct cred *cred = (struct cred *) current->cred;
	struct task_smack *tsp;

	smack_rule_cache = KMEM_CACHE(smack_rule, 0);
	if (!smack_rule_cache)
		return -ENOMEM;

	/*
	 * Set the security state for the initial task.
	 */
	tsp = smack_cred(cred);
	init_task_smack(tsp, &smack_known_floor, &smack_known_floor);

	/*
	 * Register with LSM
	 */
	security_add_hooks(smack_hooks, ARRAY_SIZE(smack_hooks), "smack");
	smack_enabled = 1;

	pr_info("Smack:  Initializing.\n");
#ifdef CONFIG_SECURITY_SMACK_NETFILTER
	pr_info("Smack:  Netfilter enabled.\n");
#endif
#ifdef SMACK_IPV6_PORT_LABELING
	pr_info("Smack:  IPv6 port labeling enabled.\n");
#endif
#ifdef SMACK_IPV6_SECMARK_LABELING
	pr_info("Smack:  IPv6 Netfilter enabled.\n");
#endif

	/* initialize the smack_known_list */
	init_smack_known_list();

	return 0;
}