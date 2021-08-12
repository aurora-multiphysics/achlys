#pragma once

// Including the "ADKernel" base class here so we can extend it
#include "ADKernel.h"

/**
 * Computes the residual contribution: K / mu * grad_test * grad_u.
 */
class ADTrappingEquilibriumEquation : public ADKernel
{
public:
  static InputParameters validParams();

  ADTrappingEquilibriumEquation(const InputParameters & parameters);

protected:
  /// ADKernel objects must override precomputeQpResidual
  //virtual 
  ADReal computeQpResidual(); // override;

  const ADVariableValue & _v;
  const ADMaterialProperty<Real> & _v_m;
  const ADMaterialProperty<Real> & _v_i;
  const ADMaterialProperty<Real> & _n_sites;

};
