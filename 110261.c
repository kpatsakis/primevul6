static int irda_getsockopt(struct socket *sock, int level, int optname,
			   char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self = irda_sk(sk);
	struct irda_device_list list;
	struct irda_device_info *discoveries;
	struct irda_ias_set *	ias_opt;	/* IAS get/query params */
	struct ias_object *	ias_obj;	/* Object in IAS */
	struct ias_attrib *	ias_attr;	/* Attribute in IAS object */
	int daddr = DEV_ADDR_ANY;	/* Dest address for IAS queries */
	int val = 0;
	int len = 0;
	int err = 0;
	int offset, total;

	pr_debug("%s(%p)\n", __func__, self);

	if (level != SOL_IRLMP)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	if(len < 0)
		return -EINVAL;

	lock_sock(sk);

	switch (optname) {
	case IRLMP_ENUMDEVICES:

		/* Offset to first device entry */
		offset = sizeof(struct irda_device_list) -
			sizeof(struct irda_device_info);

		if (len < offset) {
			err = -EINVAL;
			goto out;
		}

		/* Ask lmp for the current discovery log */
		discoveries = irlmp_get_discoveries(&list.len, self->mask.word,
						    self->nslots);
		/* Check if the we got some results */
		if (discoveries == NULL) {
			err = -EAGAIN;
			goto out;		/* Didn't find any devices */
		}

		/* Write total list length back to client */
		if (copy_to_user(optval, &list, offset))
			err = -EFAULT;

		/* Copy the list itself - watch for overflow */
		if (list.len > 2048) {
			err = -EINVAL;
			goto bed;
		}
		total = offset + (list.len * sizeof(struct irda_device_info));
		if (total > len)
			total = len;
		if (copy_to_user(optval+offset, discoveries, total - offset))
			err = -EFAULT;

		/* Write total number of bytes used back to client */
		if (put_user(total, optlen))
			err = -EFAULT;
bed:
		/* Free up our buffer */
		kfree(discoveries);
		break;
	case IRLMP_MAX_SDU_SIZE:
		val = self->max_data_size;
		len = sizeof(int);
		if (put_user(len, optlen)) {
			err = -EFAULT;
			goto out;
		}

		if (copy_to_user(optval, &val, len)) {
			err = -EFAULT;
			goto out;
		}

		break;
	case IRLMP_IAS_GET:
		/* The user want an object from our local IAS database.
		 * We just need to query the IAS and return the value
		 * that we found */

		/* Check that the user has allocated the right space for us */
		if (len != sizeof(struct irda_ias_set)) {
			err = -EINVAL;
			goto out;
		}

		/* Copy query to the driver. */
		ias_opt = memdup_user(optval, len);
		if (IS_ERR(ias_opt)) {
			err = PTR_ERR(ias_opt);
			goto out;
		}

		/* Find the object we target.
		 * If the user gives us an empty string, we use the object
		 * associated with this socket. This will workaround
		 * duplicated class name - Jean II */
		if(ias_opt->irda_class_name[0] == '\0')
			ias_obj = self->ias_obj;
		else
			ias_obj = irias_find_object(ias_opt->irda_class_name);
		if(ias_obj == (struct ias_object *) NULL) {
			kfree(ias_opt);
			err = -EINVAL;
			goto out;
		}

		/* Find the attribute (in the object) we target */
		ias_attr = irias_find_attrib(ias_obj,
					     ias_opt->irda_attrib_name);
		if(ias_attr == (struct ias_attrib *) NULL) {
			kfree(ias_opt);
			err = -EINVAL;
			goto out;
		}

		/* Translate from internal to user structure */
		err = irda_extract_ias_value(ias_opt, ias_attr->value);
		if(err) {
			kfree(ias_opt);
			goto out;
		}

		/* Copy reply to the user */
		if (copy_to_user(optval, ias_opt,
				 sizeof(struct irda_ias_set))) {
			kfree(ias_opt);
			err = -EFAULT;
			goto out;
		}
		/* Note : don't need to put optlen, we checked it */
		kfree(ias_opt);
		break;
	case IRLMP_IAS_QUERY:
		/* The user want an object from a remote IAS database.
		 * We need to use IAP to query the remote database and
		 * then wait for the answer to come back. */

		/* Check that the user has allocated the right space for us */
		if (len != sizeof(struct irda_ias_set)) {
			err = -EINVAL;
			goto out;
		}

		/* Copy query to the driver. */
		ias_opt = memdup_user(optval, len);
		if (IS_ERR(ias_opt)) {
			err = PTR_ERR(ias_opt);
			goto out;
		}

		/* At this point, there are two cases...
		 * 1) the socket is connected - that's the easy case, we
		 *	just query the device we are connected to...
		 * 2) the socket is not connected - the user doesn't want
		 *	to connect and/or may not have a valid service name
		 *	(so can't create a fake connection). In this case,
		 *	we assume that the user pass us a valid destination
		 *	address in the requesting structure...
		 */
		if(self->daddr != DEV_ADDR_ANY) {
			/* We are connected - reuse known daddr */
			daddr = self->daddr;
		} else {
			/* We are not connected, we must specify a valid
			 * destination address */
			daddr = ias_opt->daddr;
			if((!daddr) || (daddr == DEV_ADDR_ANY)) {
				kfree(ias_opt);
				err = -EINVAL;
				goto out;
			}
		}

		/* Check that we can proceed with IAP */
		if (self->iriap) {
			net_warn_ratelimited("%s: busy with a previous query\n",
					     __func__);
			kfree(ias_opt);
			err = -EBUSY;
			goto out;
		}

		self->iriap = iriap_open(LSAP_ANY, IAS_CLIENT, self,
					 irda_getvalue_confirm);

		if (self->iriap == NULL) {
			kfree(ias_opt);
			err = -ENOMEM;
			goto out;
		}

		/* Treat unexpected wakeup as disconnect */
		self->errno = -EHOSTUNREACH;

		/* Query remote LM-IAS */
		iriap_getvaluebyclass_request(self->iriap,
					      self->saddr, daddr,
					      ias_opt->irda_class_name,
					      ias_opt->irda_attrib_name);

		/* Wait for answer, if not yet finished (or failed) */
		if (wait_event_interruptible(self->query_wait,
					     (self->iriap == NULL))) {
			/* pending request uses copy of ias_opt-content
			 * we can free it regardless! */
			kfree(ias_opt);
			/* Treat signals as disconnect */
			err = -EHOSTUNREACH;
			goto out;
		}

		/* Check what happened */
		if (self->errno)
		{
			kfree(ias_opt);
			/* Requested object/attribute doesn't exist */
			if((self->errno == IAS_CLASS_UNKNOWN) ||
			   (self->errno == IAS_ATTRIB_UNKNOWN))
				err = -EADDRNOTAVAIL;
			else
				err = -EHOSTUNREACH;

			goto out;
		}

		/* Translate from internal to user structure */
		err = irda_extract_ias_value(ias_opt, self->ias_result);
		if (self->ias_result)
			irias_delete_value(self->ias_result);
		if (err) {
			kfree(ias_opt);
			goto out;
		}

		/* Copy reply to the user */
		if (copy_to_user(optval, ias_opt,
				 sizeof(struct irda_ias_set))) {
			kfree(ias_opt);
			err = -EFAULT;
			goto out;
		}
		/* Note : don't need to put optlen, we checked it */
		kfree(ias_opt);
		break;
	case IRLMP_WAITDEVICE:
		/* This function is just another way of seeing life ;-)
		 * IRLMP_ENUMDEVICES assumes that you have a static network,
		 * and that you just want to pick one of the devices present.
		 * On the other hand, in here we assume that no device is
		 * present and that at some point in the future a device will
		 * come into range. When this device arrive, we just wake
		 * up the caller, so that he has time to connect to it before
		 * the device goes away...
		 * Note : once the node has been discovered for more than a
		 * few second, it won't trigger this function, unless it
		 * goes away and come back changes its hint bits (so we
		 * might call it IRLMP_WAITNEWDEVICE).
		 */

		/* Check that the user is passing us an int */
		if (len != sizeof(int)) {
			err = -EINVAL;
			goto out;
		}
		/* Get timeout in ms (max time we block the caller) */
		if (get_user(val, (int __user *)optval)) {
			err = -EFAULT;
			goto out;
		}

		/* Tell IrLMP we want to be notified */
		irlmp_update_client(self->ckey, self->mask.word,
				    irda_selective_discovery_indication,
				    NULL, (void *) self);

		/* Do some discovery (and also return cached results) */
		irlmp_discovery_request(self->nslots);

		/* Wait until a node is discovered */
		if (!self->cachedaddr) {
			pr_debug("%s(), nothing discovered yet, going to sleep...\n",
				 __func__);

			/* Set watchdog timer to expire in <val> ms. */
			self->errno = 0;
			setup_timer(&self->watchdog, irda_discovery_timeout,
					(unsigned long)self);
			mod_timer(&self->watchdog,
				  jiffies + msecs_to_jiffies(val));

			/* Wait for IR-LMP to call us back */
			err = __wait_event_interruptible(self->query_wait,
			      (self->cachedaddr != 0 || self->errno == -ETIME));

			/* If watchdog is still activated, kill it! */
			del_timer(&(self->watchdog));

			pr_debug("%s(), ...waking up !\n", __func__);

			if (err != 0)
				goto out;
		}
		else
			pr_debug("%s(), found immediately !\n",
				 __func__);

		/* Tell IrLMP that we have been notified */
		irlmp_update_client(self->ckey, self->mask.word,
				    NULL, NULL, NULL);

		/* Check if the we got some results */
		if (!self->cachedaddr) {
			err = -EAGAIN;		/* Didn't find any devices */
			goto out;
		}
		daddr = self->cachedaddr;
		/* Cleanup */
		self->cachedaddr = 0;

		/* We return the daddr of the device that trigger the
		 * wakeup. As irlmp pass us only the new devices, we
		 * are sure that it's not an old device.
		 * If the user want more details, he should query
		 * the whole discovery log and pick one device...
		 */
		if (put_user(daddr, (int __user *)optval)) {
			err = -EFAULT;
			goto out;
		}

		break;
	default:
		err = -ENOPROTOOPT;
	}

out:

	release_sock(sk);

	return err;
}