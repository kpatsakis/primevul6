static int fastrpc_init(void)
{
	int ret;

	ret = platform_driver_register(&fastrpc_cb_driver);
	if (ret < 0) {
		pr_err("fastrpc: failed to register cb driver\n");
		return ret;
	}

	ret = register_rpmsg_driver(&fastrpc_driver);
	if (ret < 0) {
		pr_err("fastrpc: failed to register rpmsg driver\n");
		platform_driver_unregister(&fastrpc_cb_driver);
		return ret;
	}

	return 0;
}