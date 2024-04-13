hdb_samba4_check_client_matches_target_service(krb5_context context, HDB *db,
			  hdb_entry *client_entry,
			  hdb_entry *server_target_entry)
{
	struct samba_kdc_entry *skdc_client_entry
		= talloc_get_type_abort(client_entry->context,
					struct samba_kdc_entry);
	struct samba_kdc_entry *skdc_server_target_entry
		= talloc_get_type_abort(server_target_entry->context,
					struct samba_kdc_entry);

	return samba_kdc_check_client_matches_target_service(context,
							     skdc_client_entry,
							     skdc_server_target_entry);
}