static int nft_parse_register(const struct nlattr *attr, u32 *preg)
{
	unsigned int reg;

	reg = ntohl(nla_get_be32(attr));
	switch (reg) {
	case NFT_REG_VERDICT...NFT_REG_4:
		*preg = reg * NFT_REG_SIZE / NFT_REG32_SIZE;
		break;
	case NFT_REG32_00...NFT_REG32_15:
		*preg = reg + NFT_REG_SIZE / NFT_REG32_SIZE - NFT_REG32_00;
		break;
	default:
		return -ERANGE;
	}

	return 0;
}