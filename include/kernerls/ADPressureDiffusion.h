#pragma once

#include "ADMatDiffusionBase.h"

/**
 * Isotropic diffusion kernel that takes a diffusion coefficient of type
 * Real. All logic is implemnted in the ADMatDiffusionBase class
 * template.
 */
class ADPressureDiffusion : public ADMatDiffusionBase<Real>
{
public:
  static InputParameters validParams();

  ADPressureDiffusion(const InputParameters & parameters);

  protected:
  virtual ADRealVectorValue precomputeQpResidual() override;
  const ADVariableValue & _T;
//   const ADMaterialProperty<Real> & _v_m;
  const Real _R;
  const Real _Vh;
};
