static int __init elo_driver_init(void)
{
	int ret;

	wq = create_singlethread_workqueue("elousb");
	if (!wq)
		return -ENOMEM;

	ret = hid_register_driver(&elo_driver);
	if (ret)
		destroy_workqueue(wq);

	return ret;
}