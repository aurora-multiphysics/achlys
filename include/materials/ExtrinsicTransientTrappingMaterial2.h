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

class ExtrinsicTransientTrappingMaterial2;

// template <>
// InputParameters validParams<ExtrinsicTransientTrappingMaterial2>();

class ExtrinsicTransientTrappingMaterial2 : public ADMaterial
{
public:
  static InputParameters validParams();
  ExtrinsicTransientTrappingMaterial2(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

private:

  Real _v0;
  Real _E1;
  Real _E2;
  Real _E3;
  Real _k_boltz;
  Real _D0;
  Real _E_diff;
  Real _lambda;
  Real _n_sol;
  Real _const_sites_avail_type_1;
  Real _const_sites_avail_type_2;
  Real _n3a;
  Real _n3b;
  Real _eta_a;
  Real _eta_b;
  Real _flux;
  Real _conductivity;
  Real _cp;
  Real _density;
  Real _T;
  Real _xp;
  const Function & _flux_function;
  Real _theta;

  ADMaterialProperty<Real> & _D;
  ADMaterialProperty<Real> & _Vm;
  ADMaterialProperty<Real> & _V1;
  ADMaterialProperty<Real> & _V2;
  ADMaterialProperty<Real> & _V3;
  ADMaterialProperty<Real> & _n1;
  ADMaterialProperty<Real> & _n2;
  ADMaterialProperty<Real> & _n3;
  ADMaterialProperty<Real> & _Thermal_diffusivity;
  ADMaterialProperty<Real> & _Conductivity;
  ADMaterialProperty<Real> & _Density;
  ADMaterialProperty<Real> & _Cp;
};