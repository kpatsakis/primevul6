static void elo_work(struct work_struct *work)
{
	struct elo_priv *priv = container_of(work, struct elo_priv, work.work);
	struct usb_device *dev = priv->usbdev;
	unsigned char *buffer = priv->buffer;
	int ret;

	ret = elo_flush_smartset_responses(dev);
	if (ret < 0) {
		dev_err(&dev->dev, "initial FLUSH_SMARTSET_RESPONSES failed, error %d\n",
				ret);
		goto fail;
	}

	/* send Diagnostics command */
	*buffer = ELO_DIAG;
	ret = elo_smartset_send_get(dev, ELO_SEND_SMARTSET_COMMAND, buffer);
	if (ret < 0) {
		dev_err(&dev->dev, "send Diagnostics Command failed, error %d\n",
				ret);
		goto fail;
	}

	/* get the result */
	ret = elo_smartset_send_get(dev, ELO_GET_SMARTSET_RESPONSE, buffer);
	if (ret < 0) {
		dev_err(&dev->dev, "get Diagnostics Command response failed, error %d\n",
				ret);
		goto fail;
	}

	/* read the ack */
	if (*buffer != 'A') {
		ret = elo_smartset_send_get(dev, ELO_GET_SMARTSET_RESPONSE,
				buffer);
		if (ret < 0) {
			dev_err(&dev->dev, "get acknowledge response failed, error %d\n",
					ret);
			goto fail;
		}
	}

fail:
	ret = elo_flush_smartset_responses(dev);
	if (ret < 0)
		dev_err(&dev->dev, "final FLUSH_SMARTSET_RESPONSES failed, error %d\n",
				ret);
	queue_delayed_work(wq, &priv->work, ELO_PERIODIC_READ_INTERVAL);
}