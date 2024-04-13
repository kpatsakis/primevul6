bool our_curl_supports_proxy_uris()
{
	curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);
	return data->age && data->version_num >= (( 7 <<16)|( 21 <<8)| 7);  // 7.21.7
}