initoptions_finish()
{
#ifndef MAC
	char *opts = getenv("NETHACKOPTIONS");
	if (!opts) opts = getenv("HACKOPTIONS");
	if (opts) {
		if (*opts == '/' || *opts == '\\' || *opts == '@') {
			if (*opts == '@') opts++;	/* @filename */
			/* looks like a filename */
			if (strlen(opts) < BUFSZ/2)
			    read_config_file(opts, SET_IN_FILE);
		} else {
			read_config_file((char *)0, SET_IN_FILE);
			/* let the total length of options be long;
			 * parseoptions() will check each individually
			 */
			parseoptions(opts, TRUE, FALSE);
		}
	} else
#endif
		read_config_file((char *)0, SET_IN_FILE);

	(void)fruitadd(pl_fruit);
	/* Remove "slime mold" from list of object names; this will	*/
	/* prevent it from being wished unless it's actually present	*/
	/* as a named (or default) fruit.  Wishing for "fruit" will	*/
	/* result in the player's preferred fruit [better than "\033"].	*/
	obj_descr[SLIME_MOLD].oc_name = "fruit";

	return;
}