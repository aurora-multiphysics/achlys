// This file is part of the MOOSE framework
// https://www.mooseframework.org
//
// All rights reserved, see COPYRIGHT for full restrictions
// https://github.com/idaholab/moose/blob/master/COPYRIGHT
//
// Licensed under LGPL 2.1, please see LICENSE for details
// https://www.gnu.org/licenses/lgpl-2.1.html

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
  // Real _rho;
  std::vector<Real> _ni;
  Real _conductivity;
  Real _cp;
  Real _density;
  // Real _T;
  // Real _xp;

  const ADVariableValue & _T;
  ADMaterialProperty<Real> & _D;
  ADMaterialProperty<Real> & _Vm;
  ADMaterialProperty<std::vector<Real>> & _Vi;
  ADMaterialProperty<std::vector<Real>> & _n;

  ADMaterialProperty<Real> & _Thermal_diffusivity;
  ADMaterialProperty<Real> & _Conductivity;
  ADMaterialProperty<Real> & _Density;
  ADMaterialProperty<Real> & _Cp;
};