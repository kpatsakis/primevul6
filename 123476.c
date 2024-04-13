TEST(SegmentSumOpModelTest, TestFailIfSegmentsAreNotTheRightCardinality) {
  SegmentSumOpModel<int32_t> model({TensorType_INT32, {3, 2}},
                                   {TensorType_INT32, {2}});
  model.PopulateTensor<int32_t>(model.data(), {1, 2, 3, 4, 5, 6});
  model.PopulateTensor<int32_t>(model.segment_ids(), {0, 1});
  ASSERT_EQ(model.InvokeUnchecked(), kTfLiteError);
}