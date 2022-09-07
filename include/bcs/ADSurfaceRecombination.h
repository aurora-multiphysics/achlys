#pragma once
 
#include "ADIntegratedBC.h"
 
class ADSurfaceRecombination;
 
class ADSurfaceRecombination : public ADIntegratedBC
{
public:
  static InputParameters validParams();
 
  ADSurfaceRecombination(const InputParameters & parameters);
 
protected:
  virtual ADReal computeQpResidual() override;

  Real _T;
  Real _K0;
  Real _E;
  const Real _scale_factor;

};
