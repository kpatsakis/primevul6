static void std_opt_2(ArmOpcode *ao) {
	ao->a[2] = ao->a[1];
	ao->a[1] = ao->a[0];
}