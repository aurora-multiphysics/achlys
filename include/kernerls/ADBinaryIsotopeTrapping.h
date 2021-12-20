#pragma once

#include "ADKernel.h"


class ADBinaryIsotopeTrapping : public ADKernel
{
public:
  static InputParameters validParams();

  ADBinaryIsotopeTrapping(const InputParameters & parameters);

protected:
  /// ADKernel objects must override precomputeQpResidual
  //virtual 
  ADReal computeQpResidual(); // override;

  //const std::vector<const ADVariableValue *> _v;
  const ADVariableValue & _v;
  const ADVariableValue & _c2;
  const ADMaterialProperty<Real> & _v_m;
  const ADMaterialProperty<Real> & _v_i;
  const ADMaterialProperty<Real> & _n_sites;

};
