#pragma once

#include "online_predictor.h"


template <ELeavesEstimation LeafEstimationType>
inline void UpdateBucket(const TDer1Der2&, double, int, TSum*);

template <>
inline void UpdateBucket<ELeavesEstimation::Gradient>(const TDer1Der2& der, double weight, int it, TSum* bucket) {
    bucket->AddDerWeight(der.Der1, weight, it);
}

template <>
inline void UpdateBucket<ELeavesEstimation::Newton>(const TDer1Der2& der, double, int it, TSum* bucket) {
    bucket->AddDerDer2(der.Der1, der.Der2, it);
}

template <ELeavesEstimation LeafEstimationType>
inline double CalcModel(const TSum&, int, float);

template <>
inline double CalcModel<ELeavesEstimation::Gradient>(const TSum& ss, int gradientIteration, float l2Regularizer) {
    return CalcModelGradient(ss, gradientIteration, l2Regularizer);
}

template <>
inline double CalcModel<ELeavesEstimation::Newton>(const TSum& ss, int gradientIteration, float l2Regularizer) {
    return CalcModelNewton(ss, gradientIteration, l2Regularizer);
}
