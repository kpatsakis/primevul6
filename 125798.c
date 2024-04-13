static bool nft_supported_family(u8 family)
{
	return false
#ifdef CONFIG_NF_TABLES_INET
		|| family == NFPROTO_INET
#endif
#ifdef CONFIG_NF_TABLES_IPV4
		|| family == NFPROTO_IPV4
#endif
#ifdef CONFIG_NF_TABLES_ARP
		|| family == NFPROTO_ARP
#endif
#ifdef CONFIG_NF_TABLES_NETDEV
		|| family == NFPROTO_NETDEV
#endif
#if IS_ENABLED(CONFIG_NF_TABLES_BRIDGE)
		|| family == NFPROTO_BRIDGE
#endif
#ifdef CONFIG_NF_TABLES_IPV6
		|| family == NFPROTO_IPV6
#endif
		;
}