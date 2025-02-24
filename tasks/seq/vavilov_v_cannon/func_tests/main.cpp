#include <gtest/gtest.h>

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <random>

#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"
#include "seq/vavilov_v_cannon/include/ops_seq.hpp"

std::vector<double> GenerateRandomMatrix(unsigned int N, double min_val = -10.0, double max_val = 10.0) {
    std::vector<double> matrix(N * N);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min_val, max_val);

    for (unsigned int i = 0; i < N * N; i++) {
        matrix[i] = dist(gen);
    }
    return matrix;
}

std::vector<double> MultiplyMatrices(const std::vector<double>& A, const std::vector<double>& B, unsigned int N) {
    std::vector<double> C(N * N, 0.0);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            for (unsigned int k = 0; k < N; k++) {
                C[(i * N) + j] += A[(i * N) + k] * B[(k * N) + j];
            }
        }
    }
    return C;
}

TEST(vavilov_v_cannon_seq, test_random) {
  constexpr unsigned int kN = 16;
  auto a = GenerateRandomMatrix(kN);
  auto b = GenerateRandomMatrix(kN);
  std::vector<double> expected_output = MultiplyMatrices(a, b, kN);
  std::vector<double> c(kN * kN, 0.0);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_TRUE(task_seq.Validation());
  task_seq.PreProcessing();
  task_seq.Run();
  task_seq.PostProcessing();

  for (unsigned int i = 0; i < kN * kN; i++) {
    EXPECT_NEAR(expected_output[i], c[i], 1e-6);
  }
}

TEST(vavilov_v_cannon_seq, test_fixed_4x4) {
  constexpr unsigned int kN = 4;
  std::vector<double> a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  std::vector<double> b = {1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0};
  std::vector<double> expected_output = {4, 6, 6, 4, 12, 14, 14, 12, 20, 22, 22, 20, 28, 30, 30, 28};
  std::vector<double> c(kN * kN, 0.0);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_TRUE(task_seq.Validation());
  task_seq.PreProcessing();
  task_seq.Run();
  task_seq.PostProcessing();

  for (unsigned int i = 0; i < kN * kN; i++) {
    EXPECT_EQ(expected_output[i], c[i]);
  }
}

TEST(vavilov_v_cannon_seq, test_invalid_size) {
  constexpr unsigned int kN = 51;
  std::vector<double> a(kN * kN, 1.0);
  std::vector<double> b(kN * kN, 1.0);
  std::vector<double> c(kN * kN, 0.0);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_FALSE(task_seq.Validation());
}

TEST(vavilov_v_cannon_seq, test_225) {
  constexpr unsigned int kN = 225;
  std::vector<double> a(kN * kN, 1.0);
  std::vector<double> b(kN * kN, 1.0);
  std::vector<double> c(kN * kN, 0.0);
  std::vector<double> expected_output(kN * kN, kN);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_TRUE(task_seq.Validation());
  task_seq.PreProcessing();
  task_seq.Run();
  task_seq.PostProcessing();

  for (unsigned int i = 0; i < kN * kN; i++) {
    EXPECT_EQ(expected_output[i], c[i]);
  }
}

TEST(vavilov_v_cannon_seq, test_225_from_file) {
  std::string line;
  std::ifstream test_file(ppc::util::GetAbsolutePath("seq/vavilov_v_cannon/data/test.TXT"));
  unsigned int k_n = 0;
  if (test_file.is_open()) {
    getline(test_file, line);
  }
  test_file.close();

  k_n = std::stoi(line);

  std::vector<double> a(k_n * k_n, 1.0);
  std::vector<double> b(k_n * k_n, 1.0);
  std::vector<double> c(k_n * k_n, 0.0);
  std::vector<double> expected_output(k_n * k_n, k_n);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_TRUE(task_seq.Validation());
  task_seq.PreProcessing();
  task_seq.Run();
  task_seq.PostProcessing();

  for (unsigned int i = 0; i < k_n * k_n; i++) {
    EXPECT_EQ(expected_output[i], c[i]);
  }
}

TEST(vavilov_v_cannon_seq, test_identity_matrix) {
  constexpr unsigned int kN = 225;
  std::vector<double> a(kN * kN, 1.0);
  std::vector<double> b(kN * kN, 0.0);
  std::vector<double> c(kN * kN, 0.0);

  for (unsigned int i = 0; i < kN; i++) {
    b[(i * kN) + i] = 1.0;
  }

  auto expected_output = a;

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_TRUE(task_seq.Validation());
  task_seq.PreProcessing();
  task_seq.Run();
  task_seq.PostProcessing();

  for (unsigned int i = 0; i < kN * kN; i++) {
    EXPECT_EQ(expected_output[i], c[i]);
  }
}

TEST(vavilov_v_cannon_seq, test_zero_matrix) {
  constexpr unsigned int kN = 225;
  std::vector<double> a(kN * kN, 1.0);
  std::vector<double> b(kN * kN, 0.0);
  std::vector<double> c(kN * kN, 0.0);
  std::vector<double> expected_output(kN * kN, 0.0);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(a.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(b.data()));
  task_data_seq->inputs_count.emplace_back(a.size());
  task_data_seq->inputs_count.emplace_back(b.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(c.data()));
  task_data_seq->outputs_count.emplace_back(c.size());

  vavilov_v_cannon_seq::CannonSequential task_seq(task_data_seq);
  ASSERT_TRUE(task_seq.Validation());
  task_seq.PreProcessing();
  task_seq.Run();
  task_seq.PostProcessing();

  for (unsigned int i = 0; i < kN * kN; i++) {
    EXPECT_EQ(expected_output[i], c[i]);
  }
}
