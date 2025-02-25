static uint64_t virtio_net_get_features(VirtIODevice *vdev, uint64_t features,
                                        Error **errp)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    NetClientState *nc = qemu_get_queue(n->nic);

    /* Firstly sync all virtio-net possible supported features */
    features |= n->host_features;

    virtio_add_feature(&features, VIRTIO_NET_F_MAC);

    if (!peer_has_vnet_hdr(n)) {
        virtio_clear_feature(&features, VIRTIO_NET_F_CSUM);
        virtio_clear_feature(&features, VIRTIO_NET_F_HOST_TSO4);
        virtio_clear_feature(&features, VIRTIO_NET_F_HOST_TSO6);
        virtio_clear_feature(&features, VIRTIO_NET_F_HOST_ECN);

        virtio_clear_feature(&features, VIRTIO_NET_F_GUEST_CSUM);
        virtio_clear_feature(&features, VIRTIO_NET_F_GUEST_TSO4);
        virtio_clear_feature(&features, VIRTIO_NET_F_GUEST_TSO6);
        virtio_clear_feature(&features, VIRTIO_NET_F_GUEST_ECN);

        virtio_clear_feature(&features, VIRTIO_NET_F_HASH_REPORT);
    }

    if (!peer_has_vnet_hdr(n) || !peer_has_ufo(n)) {
        virtio_clear_feature(&features, VIRTIO_NET_F_GUEST_UFO);
        virtio_clear_feature(&features, VIRTIO_NET_F_HOST_UFO);
    }

    if (!get_vhost_net(nc->peer)) {
        return features;
    }

    if (!ebpf_rss_is_loaded(&n->ebpf_rss)) {
        virtio_clear_feature(&features, VIRTIO_NET_F_RSS);
    }
    features = vhost_net_get_features(get_vhost_net(nc->peer), features);
    vdev->backend_features = features;

    if (n->mtu_bypass_backend &&
            (n->host_features & 1ULL << VIRTIO_NET_F_MTU)) {
        features |= (1ULL << VIRTIO_NET_F_MTU);
    }

    return features;
}