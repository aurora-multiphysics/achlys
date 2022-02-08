// This file is part of the MOOSE framework
// https://www.mooseframework.org
#pragma once

#include "ADMaterial.h"
#include "LinearInterpolation.h"

class VectorTrappingMaterial;

template <>
InputParameters validParams<VectorTrappingMaterial>();

class VectorTrappingMaterial : public ADMaterial
{
public:
  VectorTrappingMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  ADReal Arhhenious(Real v0, Real E);

private:

  //std::vector<const VariableValue *> _simulation_temperature;
  std::vector<Real> _v0;
  std::vector<Real> _Ei;
  Real _k_boltz;
  Real _D0;
  Real _E_diff;
  Real _lambda;
  Real _n_sol;
  std::vector<Real> _ni;

  const ADVariableValue & _T;
  ADMaterialProperty<Real> & _D;
  ADMaterialProperty<Real> & _Vm;
  ADMaterialProperty<std::vector<Real>> & _Vi;
  ADMaterialProperty<std::vector<Real>> & _n;
};