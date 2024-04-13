static void fastrpc_exit(void)
{
	platform_driver_unregister(&fastrpc_cb_driver);
	unregister_rpmsg_driver(&fastrpc_driver);
}