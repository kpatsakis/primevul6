static void elo_remove(struct hid_device *hdev)
{
	struct elo_priv *priv = hid_get_drvdata(hdev);

	usb_put_dev(priv->usbdev);

	hid_hw_stop(hdev);
	cancel_delayed_work_sync(&priv->work);
	kfree(priv);
}