e1000_receive(NetClientState *nc, const uint8_t *buf, size_t size)
{
    const struct iovec iov = {
        .iov_base = (uint8_t *)buf,
        .iov_len = size
    };

    return e1000_receive_iov(nc, &iov, 1);
}