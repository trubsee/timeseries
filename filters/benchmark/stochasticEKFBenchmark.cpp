#include "benchmark/benchmark.h"

#include "common/randomSample.hpp"
#include "filters/stochasticEKF.ipp"
#include "stochasticModels/linearFactoryMethods.hpp"

namespace Filters::Benchmark {

static void BenchmarkStochasticEKF(benchmark::State& state)
{
    const double initial{0.};
    const double drift{0.5};
    const double noise{0.1};
    const auto stateModel{
        StochasticModels::CreateRandomWalk(Eigen::MatrixXd{{drift}})
    };
    const auto obsModel{
        StochasticModels::CreateRandomWalk(Eigen::MatrixXd{{noise}})
    };

    StochasticEKF<500> ekf {
        Eigen::MatrixXd{{initial}},
        Eigen::MatrixXd{{1}},
        stateModel,
        obsModel
    };

    const unsigned ITER{1000};

    for (auto _ : state)
    {
        auto hiddenState = initial + Common::SampleNormal() * std::sqrt(drift);

        for (unsigned i = 0; i < ITER; ++i)
        {
            // generate fake obs
            auto obs = hiddenState + Common::SampleNormal() * std::sqrt(noise);
            ekf.Update(Eigen::VectorXd{{obs}});
        }
    }
}
BENCHMARK(BenchmarkStochasticEKF);

}

BENCHMARK_MAIN();
