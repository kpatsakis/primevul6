static void credential_apply_config(struct credential *c)
{
	if (c->configured)
		return;
	git_config(credential_config_callback, c);
	c->configured = 1;

	if (!c->use_http_path && proto_is_http(c->protocol)) {
		FREE_AND_NULL(c->path);
	}
}