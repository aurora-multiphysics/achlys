#pragma once

#include "ADMaterial.h"
#include "LinearInterpolation.h"

class TrappingMaterialRampingT;

class TrappingMaterialRampingT : public ADMaterial
{
public:
  static InputParameters validParams();
  TrappingMaterialRampingT(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

private:

  //std::vector<const VariableValue *> _simulation_temperature;
  Real _v0;
  Real _E1;
  Real _E2;
  Real _E3;
  Real _k_boltz;
  Real _D0;
  Real _E_diff;
  Real _lambda;
  Real _n_sol;
  Real _rho;
  Real _const_sites_avail_type_1;
  Real _const_sites_avail_type_2;
  Real _const_sites_avail_type_3;
  Real _conductivity;
  Real _cp;
  Real _density;
  Real _T_start;
  Real _beta;
  Real _xp;

  // const ADVariableValue & _T;
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
