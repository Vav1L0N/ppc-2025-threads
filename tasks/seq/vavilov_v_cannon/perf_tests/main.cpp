#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"
#include "seq/vavilov_v_cannon/include/ops_seq.hpp"

TEST(vavilov_v_cannon_seq, test_pipeline_run) {
  constexpr int N = 500;
  std::vector<double> A(N * N, 1.0);
  std::vector<double> B(N * N, 1.0);
  std::vector<double> C(N * N, 0.0);
  std::vector<double> expected_output(N * N, N);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(A.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(B.data()));
  task_data_seq->inputs_count.emplace_back(A.size());
  task_data_seq->inputs_count.emplace_back(B.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(C.data()));
  task_data_seq->outputs_count.emplace_back(C.size());

  auto task_seq = std::make_shared<vavilov_v_cannon_seq::CannonSequential>(task_data_seq);

  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  auto perf_analyzer = std::make_shared<ppc::core::Perf>(task_seq);
  perf_analyzer->PipelineRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  for (size_t i = 0; i < N * N; i++) {
    ASSERT_EQ(expected_output[i], C[i]);
  }
}

TEST(vavilov_v_cannon_seq, test_task_run) {
  constexpr int N = 500;
  std::vector<double> A(N * N, 1.0);
  std::vector<double> B(N * N, 1.0);
  std::vector<double> C(N * N, 0.0);
  std::vector<double> expected_output(N * N, N);

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(A.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t*>(B.data()));
  task_data_seq->inputs_count.emplace_back(A.size());
  task_data_seq->inputs_count.emplace_back(B.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t*>(C.data()));
  task_data_seq->outputs_count.emplace_back(C.size());

  auto task_seq = std::make_shared<vavilov_v_cannon_seq::CannonSequential>(task_data_seq);

  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  auto perf_analyzer = std::make_shared<ppc::core::Perf>(task_seq);
  perf_analyzer->TaskRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  for (size_t i = 0; i < N * N; i++) {
    ASSERT_EQ(expected_output[i], C[i]);
  }
}
