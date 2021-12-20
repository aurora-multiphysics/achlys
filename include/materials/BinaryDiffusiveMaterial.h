#pragma once

#include "ADMaterial.h"
#include "LinearInterpolation.h"

class BinaryDiffusiveMaterial;

template <>
InputParameters validParams<BinaryDiffusiveMaterial>();

class BinaryDiffusiveMaterial : public ADMaterial
{
public:
  BinaryDiffusiveMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

private:

  Real _k_boltz;
  Real _D0_1;
  Real _E_diff_1;
  Real _D0_2;
  Real _E_diff_2;
  Real _lambda;
  Real _n_sol;

  const ADVariableValue & _T;
  ADMaterialProperty<Real> & _D1;
  ADMaterialProperty<Real> & _D2;
  ADMaterialProperty<Real> & _Vm1;
  ADMaterialProperty<Real> & _Vm2;
};