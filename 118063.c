dotogglepickup()
{
	char buf[BUFSZ], ocl[MAXOCLASSES+1];

	flags.pickup = !flags.pickup;
	if (flags.pickup) {
	    oc_to_str(flags.pickup_types, ocl);
	    Sprintf(buf, "ON, for %s objects%s", ocl[0] ? ocl : "all",
#ifdef AUTOPICKUP_EXCEPTIONS
			(iflags.autopickup_exceptions[AP_LEAVE] ||
			 iflags.autopickup_exceptions[AP_GRAB]) ?
			 ((count_ape_maps((int *)0, (int *)0) == 1) ?
			    ", with one exception" : ", with some exceptions") :
#endif
			"");
	} else {
	    Strcpy(buf, "OFF");
	}
	pline("Autopickup: %s.", buf);
	return 0;
}