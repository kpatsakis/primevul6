TEST(SegmentSumOpModelTest, Float32Test_OneDimension) {
  SegmentSumOpModel<float> model({TensorType_FLOAT32, {3}},
                                 {TensorType_INT32, {3}});
  model.PopulateTensor<float>(model.data(), {1, 2, 3});
  model.PopulateTensor<int32_t>(model.segment_ids(), {0, 0, 1});
  model.Invoke();
  EXPECT_THAT(model.GetOutput(), ElementsAreArray({3.0f, 3.0f}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({2}));
}