/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "Module.h"

namespace fl {

/**
 * Applies Batch Normalization on a given input as described in the paper
 * [Batch Normalization: Accelerating Deep Network Training by Reducing Internal
 * Covariate Shift](https://arxiv.org/abs/1502.01852).
 *
 * The operation implemented is:
 * \f[out(x) = \frac{x - E[x]}{\sqrt{Var[x]+\epsilon}} \times \gamma + \beta \f]
 * where \f$E[x]\f$ and \f$Var[x]\f$ are the mean and variance of the input
 * \f$x\f$ calculated per specified axes, \f$\epsilon\f$ is a small value
 * added to the variance to avoid divide-by-zero, and \f$\gamma\f$ and
 * \f$\beta\f$ are learnable parameters for affine transformation.
 */
class BatchNorm : public Module {
 private:
  BatchNorm() = default; // intentionally private

  std::vector<int> featAxes_;
  intl featSize_;
  intl numBatchesTracked_;
  Variable runningMean_, runningVar_;
  double momentum_, epsilon_;
  bool affine_, trackStats_;

  FL_SAVE_LOAD_WITH_BASE(
      Module,
      featAxes_,
      featSize_,
      numBatchesTracked_,
      runningMean_,
      runningVar_,
      momentum_,
      epsilon_,
      affine_,
      trackStats_)

  /**
   * Called in the constructor to initialize parameters for running mean and
   * variance if `track_stats` is set to `true`,
   * and \f$\gamma\f$ and \f$\beta\f$ as learnable parameters if
   * `affine` is set to `true` in the constructor.
   */
  void initialize();

 public:
  /**
   * Constructs a BatchNorm module.
   *
   * @param feat_axis the axis over which per-dimension normalization
   *  is performed
   * @param feat_size the size of the dimension along `feat_axis`
   * @param momentum an exponential average factor used to compute running mean
   *  and variance.
   *  \f[ runningMean = runningMean \times (1-momentum)
   *  + newMean \times momentum \f]
   *  If < 0, cumulative moving average is used.
   * @param eps \f$\epsilon\f$
   * @param affine a boolean value that controls the learning of \f$\gamma\f$
   *  and \f$\beta\f$. \f$\gamma\f$ and \f$\beta\f$ are set to 1, 0 respectively
   *  if set to `false`, or initialized as learnable parameters
   *  if set to `true`.
   * @param track_stats a boolean value that controls whether to track the
   *  running mean and variance while in train mode. If `false`, batch
   *  statistics are used to perform normalization in both train and eval mode.
   */
  BatchNorm(
      int feat_axis,
      intl feat_size,
      double momentum = 0.1,
      double eps = 1e-5,
      bool affine = true,
      bool track_stats = true);

  /**
   * Constructs a BatchNorm module.
   *
   * @param feat_axes the axes over which per-dimension normalization
   *  is performed
   * @param feat_size total dimension along `feat_axes`.
   *  For example, to perform Temporal Batch Normalization on input of size
   *  [\f$L\f$, \f$C\f$, \f$N\f$], use `feat_axes` = {1}, `feat_size` = \f$C\f$.
   *  To perform normalization per activation on input of size
   *  [\f$W\f$, \f$H\f$, \f$C\f$, \f$N\f$], use `feat_axes` = {0, 1, 2},
   *  `feat_size` = \f$W \times H \times C\f$.
   * @param momentum an exponential average factor used to compute running mean
   *  and variance.
   *  \f[ runningMean = runningMean \times (1-momentum)
   *  + newMean \times momentum \f]
   *  If < 0, cumulative moving average is used.
   * @param eps \f$\epsilon\f$
   * @param affine a boolean value that controls the learning of \f$\gamma\f$
   *  and \f$\beta\f$. \f$\gamma\f$ and \f$\beta\f$ are set to 1, 0 respectively
   *  if set to `false`, or initialized as learnable parameters
   *  if set to `true`.
   * @param track_stats a boolean value that controls whether to track the
   *  running mean and variance while in train mode. If `false`, batch
   *  statistics are used to perform normalization in both train and eval mode.
   */
  BatchNorm(
      const std::vector<int>& feat_axes,
      intl feat_size,
      double momentum = 0.1,
      double eps = 1e-5,
      bool affine = true,
      bool track_stats = true);

  Variable forward(const Variable& input) override;

  std::string prettyString() const override;
};

} // namespace fl

CEREAL_REGISTER_TYPE(fl::BatchNorm)
