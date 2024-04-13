static void std_opt_3(ArmOpcode *ao) {
	ao->a[3] = ao->a[2];
	ao->a[2] = ao->a[1];
	ao->a[1] = ao->a[0];
}