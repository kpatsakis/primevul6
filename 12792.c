static __init void init_smack_known_list(void)
{
	/*
	 * Initialize rule list locks
	 */
	mutex_init(&smack_known_huh.smk_rules_lock);
	mutex_init(&smack_known_hat.smk_rules_lock);
	mutex_init(&smack_known_floor.smk_rules_lock);
	mutex_init(&smack_known_star.smk_rules_lock);
	mutex_init(&smack_known_web.smk_rules_lock);
	/*
	 * Initialize rule lists
	 */
	INIT_LIST_HEAD(&smack_known_huh.smk_rules);
	INIT_LIST_HEAD(&smack_known_hat.smk_rules);
	INIT_LIST_HEAD(&smack_known_star.smk_rules);
	INIT_LIST_HEAD(&smack_known_floor.smk_rules);
	INIT_LIST_HEAD(&smack_known_web.smk_rules);
	/*
	 * Create the known labels list
	 */
	smk_insert_entry(&smack_known_huh);
	smk_insert_entry(&smack_known_hat);
	smk_insert_entry(&smack_known_star);
	smk_insert_entry(&smack_known_floor);
	smk_insert_entry(&smack_known_web);
}