avrcp_parse_media_player_item(struct avrcp *session, uint8_t *operands,
							uint16_t len)
{
	struct avrcp_player *player;
	struct media_player *mp;
	uint16_t id, namelen;
	uint32_t subtype;
	const char *curval, *strval;
	char name[255];

	if (len < 28)
		return NULL;

	id = get_be16(&operands[0]);

	player = find_ct_player(session, id);
	if (player == NULL) {
		player = create_ct_player(session, id);
		if (player == NULL)
			return NULL;
	} else if (player->features != NULL)
		return player;

	mp = player->user_data;

	media_player_set_type(mp, type_to_string(operands[2]));

	subtype = get_be32(&operands[3]);

	media_player_set_subtype(mp, subtype_to_string(subtype));

	curval = media_player_get_status(mp);
	strval = status_to_string(operands[7]);

	if (g_strcmp0(curval, strval) != 0) {
		media_player_set_status(mp, strval);
		avrcp_get_play_status(session);
	}

	avrcp_player_parse_features(player, &operands[8]);

	namelen = get_be16(&operands[26]);
	if (namelen > 0 && namelen + 28 == len) {
		namelen = MIN(namelen, sizeof(name) - 1);
		memcpy(name, &operands[28], namelen);
		name[namelen] = '\0';
		media_player_set_name(mp, name);
	}

	if (player->addressed)
		set_browsed_player(session, player);

	return player;
}