#pragma once

// Including the "ADKernel" base class here so we can extend it
#include "ADKernel.h"

/**
 * Computes the residual contribution: K / mu * grad_test * grad_u.
 */
class ADSurfaceAdsorption : public ADKernel
{
public:
  static InputParameters validParams();

  ADSurfaceAdsorption(const InputParameters & parameters);

protected:
  /// ADKernel objects must override precomputeQpResidual
  //virtual 
  ADReal computeQpResidual(); // override;

  //const std::vector<const ADVariableValue *> _v;
  const ADMaterialProperty<Real>  & _gamma;
  const ADMaterialProperty<Real> & _vd;
  const ADMaterialProperty<Real> & _vsb;
  const ADMaterialProperty<Real> & _sigma;
  const ADMaterialProperty<Real> & _n_surf;
  const ADMaterialProperty<Real> & _s;

};
