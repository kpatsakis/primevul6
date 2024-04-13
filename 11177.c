static void __exit elo_driver_exit(void)
{
	hid_unregister_driver(&elo_driver);
	destroy_workqueue(wq);
}