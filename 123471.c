TEST(SegmentSumOpModelTest, Int32Test_OneDimension) {
  SegmentSumOpModel<int32_t> model({TensorType_INT32, {3}},
                                   {TensorType_INT32, {3}});
  model.PopulateTensor<int32_t>(model.data(), {1, 2, 3});
  model.PopulateTensor<int32_t>(model.segment_ids(), {0, 0, 1});
  model.Invoke();
  EXPECT_THAT(model.GetOutput(), ElementsAreArray({3, 3}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({2}));
}