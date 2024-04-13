theme_adium_append_message (EmpathyChatView *view,
			    EmpathyMessage  *msg)
{
	EmpathyThemeAdium     *theme = EMPATHY_THEME_ADIUM (view);
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	EmpathyContact        *sender;
	TpMessage             *tp_msg;
	TpAccount             *account;
	gchar                 *body_escaped, *name_escaped;
	const gchar           *name;
	const gchar           *contact_id;
	EmpathyAvatar         *avatar;
	const gchar           *avatar_filename = NULL;
	gint64                 timestamp;
	const gchar           *html = NULL;
	const gchar           *func;
	const gchar           *service_name;
	GString               *message_classes = NULL;
	gboolean               is_backlog;
	gboolean               consecutive;
	gboolean               action;

	if (priv->pages_loading != 0) {
		queue_item (&priv->message_queue, QUEUED_MESSAGE, msg, NULL);
		return;
	}

	/* Get information */
	sender = empathy_message_get_sender (msg);
	account = empathy_contact_get_account (sender);
	service_name = empathy_protocol_name_to_display_name
		(tp_account_get_protocol (account));
	if (service_name == NULL)
		service_name = tp_account_get_protocol (account);
	timestamp = empathy_message_get_timestamp (msg);
	body_escaped = theme_adium_parse_body (theme,
		empathy_message_get_body (msg),
		empathy_message_get_token (msg));
	name = empathy_contact_get_logged_alias (sender);
	contact_id = empathy_contact_get_id (sender);
	action = (empathy_message_get_tptype (msg) == TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION);

	/* If this is a /me probably */
	if (action) {
		gchar *str;

		if (priv->data->version >= 4 || !priv->data->custom_template) {
			str = g_strdup_printf ("<span class='actionMessageUserName'>%s</span>"
					       "<span class='actionMessageBody'>%s</span>",
					       name, body_escaped);
		} else {
			str = g_strdup_printf ("*%s*", body_escaped);
		}
		g_free (body_escaped);
		body_escaped = str;
	}

	/* Get the avatar filename, or a fallback */
	avatar = empathy_contact_get_avatar (sender);
	if (avatar) {
		avatar_filename = avatar->filename;
	}
	if (!avatar_filename) {
		if (empathy_contact_is_user (sender)) {
			avatar_filename = priv->data->default_outgoing_avatar_filename;
		} else {
			avatar_filename = priv->data->default_incoming_avatar_filename;
		}
		if (!avatar_filename) {
			if (!priv->data->default_avatar_filename) {
				priv->data->default_avatar_filename =
					empathy_filename_from_icon_name (EMPATHY_IMAGE_AVATAR_DEFAULT,
									 GTK_ICON_SIZE_DIALOG);
			}
			avatar_filename = priv->data->default_avatar_filename;
		}
	}

	/* We want to join this message with the last one if
	 * - senders are the same contact,
	 * - last message was recieved recently,
	 * - last message and this message both are/aren't backlog, and
	 * - DisableCombineConsecutive is not set in theme's settings */
	is_backlog = empathy_message_is_backlog (msg);
	consecutive = empathy_contact_equal (priv->last_contact, sender) &&
		(timestamp - priv->last_timestamp < MESSAGE_JOIN_PERIOD) &&
		(is_backlog == priv->last_is_backlog) &&
		!tp_asv_get_boolean (priv->data->info,
				     "DisableCombineConsecutive", NULL);

	/* Define message classes */
	message_classes = g_string_new ("message");
	if (!priv->has_focus && !is_backlog) {
		if (!priv->has_unread_message) {
			g_string_append (message_classes, " firstFocus");
			priv->has_unread_message = TRUE;
		}
		g_string_append (message_classes, " focus");
	}
	if (is_backlog) {
		g_string_append (message_classes, " history");
	}
	if (consecutive) {
		g_string_append (message_classes, " consecutive");
	}
	if (empathy_contact_is_user (sender)) {
		g_string_append (message_classes, " outgoing");
	} else {
		g_string_append (message_classes, " incoming");
	}
	if (empathy_message_should_highlight (msg)) {
		g_string_append (message_classes, " mention");
	}
	if (empathy_message_get_tptype (msg) == TP_CHANNEL_TEXT_MESSAGE_TYPE_AUTO_REPLY) {
		g_string_append (message_classes, " autoreply");
	}
	if (action) {
		g_string_append (message_classes, " action");
	}
	/* FIXME: other classes:
	 * status - the message is a status change
	 * event - the message is a notification of something happening
	 *         (for example, encryption being turned on)
	 * %status% - See %status% in theme_adium_append_html ()
	 */

	/* This is slightly a hack, but it's the only way to add
	 * arbitrary data to messages in the HTML. We add another
	 * class called "x-empathy-message-id-*" to the message. This
	 * way, we can remove the unread marker for this specific
	 * message later. */
	tp_msg = empathy_message_get_tp_message (msg);
	if (tp_msg != NULL) {
		guint32 id;
		gboolean valid;

		id = tp_message_get_pending_message_id (tp_msg, &valid);
		if (valid) {
			g_string_append_printf (message_classes,
			    " x-empathy-message-id-%u", id);
		}
	}

	/* Define javascript function to use */
	if (consecutive) {
		func = priv->allow_scrolling ? "appendNextMessage" : "appendNextMessageNoScroll";
	} else {
		func = priv->allow_scrolling ? "appendMessage" : "appendMessageNoScroll";
	}

	if (empathy_contact_is_user (sender)) {
		/* out */
		if (is_backlog) {
			/* context */
			html = consecutive ? priv->data->out_nextcontext_html : priv->data->out_context_html;
		} else {
			/* content */
			html = consecutive ? priv->data->out_nextcontent_html : priv->data->out_content_html;
		}

		/* remove all the unread marks when we are sending a message */
		theme_adium_remove_all_focus_marks (theme);
	} else {
		/* in */
		if (is_backlog) {
			/* context */
			html = consecutive ? priv->data->in_nextcontext_html : priv->data->in_context_html;
		} else {
			/* content */
			html = consecutive ? priv->data->in_nextcontent_html : priv->data->in_content_html;
		}
	}

	name_escaped = g_markup_escape_text (name, -1);

	theme_adium_append_html (theme, func, html, body_escaped,
				 avatar_filename, name_escaped, contact_id,
				 service_name, message_classes->str,
				 timestamp, is_backlog, empathy_contact_is_user (sender));

	/* Keep the sender of the last displayed message */
	if (priv->last_contact) {
		g_object_unref (priv->last_contact);
	}
	priv->last_contact = g_object_ref (sender);
	priv->last_timestamp = timestamp;
	priv->last_is_backlog = is_backlog;

	g_free (body_escaped);
	g_free (name_escaped);
	g_string_free (message_classes, TRUE);
}