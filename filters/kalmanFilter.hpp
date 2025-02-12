#pragma once

#include <Eigen/Dense>

#include "common/assert.hpp"
#include "filters/stateSpaceModelTraits.hpp"
#include "stochasticModels/linearGaussian.hpp"

namespace Filters {

template <int Hidden, int Observed>
class KalmanFilter {
   public:
    using Traits = StateSpaceModelTraits<Hidden, Observed>;
    using HiddenMatrix = typename Traits::HiddenMatrix;
    using ObsMatrix = typename Traits::ObsMatrix;
    using ObsNoiseMatrix = typename Traits::ObsNoiseMatrix;
    using HiddenVector = typename Traits::HiddenVector;
    using ObsVector = typename Traits::ObsVector;

    KalmanFilter(const HiddenVector& stateEstimate,
                 const HiddenMatrix& covEstimate, const HiddenMatrix& f,
                 const HiddenMatrix& q, const ObsMatrix& h,
                 const ObsNoiseMatrix& r)
        : F{f},
          H{h},
          Q{q},
          R{r},
          x{stateEstimate},
          P{covEstimate},
          y{Observed},
          S{Observed, Observed} {
        ASSERT(stateEstimate.size() == Hidden);
        ASSERT(covEstimate.rows() == covEstimate.cols());
        ASSERT(covEstimate.rows() == Hidden);
        // ASSERT(stateModel.GetNumOutputs() == stateModel.GetNumInputs());
        // ASSERT(stateModel.GetNumOutputs() == Hidden);
    }

    KalmanFilter(const HiddenVector& stateEstimate,
                 const HiddenMatrix& covEstimate,
                 const typename Traits::HiddenModel& stateModel,
                 const typename Traits::ObservedModel& obsModel)
        : KalmanFilter{stateEstimate,
                       covEstimate,
                       stateModel.GetCoefMatrix(),
                       stateModel.GetNoiseMatrix(),
                       obsModel.GetCoefMatrix(),
                       obsModel.GetNoiseMatrix()} {}

    void Update(const ObsVector& obs) {
        ASSERT(obs.size() == Observed);

        // Predict
        const auto xPred = Predict();
        const auto PPred = F.transpose() * P * F + Q;

        // Update
        y = obs - H * xPred;
        S = H * PPred * H.transpose() + R;
        const auto K = PPred * H.transpose() * S.inverse();
        x = xPred + K * y;
        P = (HiddenMatrix::Identity(Hidden, Hidden) - K * H) * P;
    }

    const HiddenVector& Estimate() const { return x; }

    HiddenVector Predict() const { return F * x; }

    std::pair<const ObsVector&, const ObsNoiseMatrix&> GetLastInnovation()
        const {
        return {y, S};
    }

   private:
    // model params
    HiddenMatrix F;
    ObsMatrix H;
    HiddenMatrix Q;
    ObsNoiseMatrix R;

    // current estimate
    HiddenVector x;
    HiddenMatrix P;
    // innovations
    ObsVector y;
    ObsNoiseMatrix S;
};

}  // namespace Filters
