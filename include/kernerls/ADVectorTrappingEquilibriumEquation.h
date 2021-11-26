#pragma once

// Including the "ADKernel" base class here so we can extend it
#include "ADKernel.h"

/**
 * Computes the residual contribution: K / mu * grad_test * grad_u.
 */
class ADVectorTrappingEquilibriumEquation : public ADKernel
{
public:
  static InputParameters validParams();

  ADVectorTrappingEquilibriumEquation(const InputParameters & parameters);

protected:
  /// ADKernel objects must override precomputeQpResidual
  //virtual 
  ADReal computeQpResidual(); // override;

  const ADVariableValue & _v;
  const ADMaterialProperty<Real> & _v_m;
  const ADMaterialProperty<std::vector<Real>> & _v_i;
  const ADMaterialProperty<std::vector<Real>> & _n_sites;
  Real _idx;

};
