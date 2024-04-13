TEST_F(HTTP2CodecTest, VirtualNodes) {
  DummyQueue queue;
  uint8_t level = 30;
  upstreamCodec_.addPriorityNodes(queue, output_, level);

  EXPECT_TRUE(parse());
  for (int i = 0; i < level; i++) {
    EXPECT_EQ(queue.nodes_[i], upstreamCodec_.mapPriorityToDependency(i));
  }

  // Out-of-range priorites are mapped to the lowest level of virtual nodes.
  EXPECT_EQ(queue.nodes_[level - 1],
            upstreamCodec_.mapPriorityToDependency(level));
  EXPECT_EQ(queue.nodes_[level - 1],
            upstreamCodec_.mapPriorityToDependency(level + 1));
}