bool subscribe_extranonce(struct pool *pool)
{
	json_t *val = NULL, *res_val, *err_val;
	char s[RBUFSIZE], *sret = NULL;
	json_error_t err;
	bool ret = false;

	sprintf(s, "{\"id\": %d, \"method\": \"mining.extranonce.subscribe\", \"params\": []}",
		swork_id++);

	if (!stratum_send(pool, s, strlen(s)))
		return ret;

	/* Parse all data in the queue and anything left should be the response */
	while (42) {
		if (!socket_full(pool, DEFAULT_SOCKWAIT / 30)) {
			applog(LOG_DEBUG, "Timed out waiting for response extranonce.subscribe");
			/* some pool doesnt send anything, so this is normal */
			ret = true;
			goto out;
		}

		sret = recv_line(pool);
		if (!sret)
			return ret;
		if (parse_method(pool, sret))
			free(sret);
		else
			break;
	}

	val = JSON_LOADS(sret, &err);
	free(sret);
	res_val = json_object_get(val, "result");
	err_val = json_object_get(val, "error");

	if (!res_val || json_is_false(res_val) || (err_val && !json_is_null(err_val)))  {
		char *ss;

		if (err_val) {
			ss = __json_array_string(err_val, 1);
			if (!ss)
				ss = (char *)json_string_value(err_val);
			if (ss && (strcmp(ss, "Method 'subscribe' not found for service 'mining.extranonce'") == 0)) {
				applog(LOG_INFO, "Cannot subscribe to mining.extranonce on %s", get_pool_name(pool));
				ret = true;
				goto out;
			}
			if (ss && (strcmp(ss, "Unrecognized request provided") == 0)) {
				applog(LOG_INFO, "Cannot subscribe to mining.extranonce on %s", get_pool_name(pool));
				ret = true;
				goto out;
			}
			ss = json_dumps(err_val, JSON_INDENT(3));
		}
		else
			ss = strdup("(unknown reason)");
		applog(LOG_INFO, "%s JSON stratum auth failed: %s", get_pool_name(pool), ss);
		free(ss);

		goto out;
	}

	ret = true;
	applog(LOG_INFO, "Stratum extranonce subscribe for %s", get_pool_name(pool));

out:
	json_decref(val);
	return ret;
}