#pragma once

#include "ADKernelGrad.h"

class ADCoupledIsotopeDiffusion : public ADKernelGrad
{
public:
  static InputParameters validParams();

  ADCoupledIsotopeDiffusion(const InputParameters & parameters);

protected:
  virtual ADRealVectorValue precomputeQpResidual() override;

private:
const ADVariableValue & _v;
const ADVariableGradient & _grad_v;
const ADMaterialProperty<Real> & _D1;
const ADMaterialProperty<Real> & _D2;

};
