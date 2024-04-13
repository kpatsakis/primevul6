static void __tnc_set_sync_state(struct sixpack *sp, int new_tnc_state)
{
	char *msg;

	switch (new_tnc_state) {
	default:			/* gcc oh piece-o-crap ... */
	case TNC_UNSYNC_STARTUP:
		msg = "Synchronizing with TNC";
		break;
	case TNC_UNSYNCED:
		msg = "Lost synchronization with TNC\n";
		break;
	case TNC_IN_SYNC:
		msg = "Found TNC";
		break;
	}

	sp->tnc_state = new_tnc_state;
	printk(KERN_INFO "%s: %s\n", sp->dev->name, msg);
}