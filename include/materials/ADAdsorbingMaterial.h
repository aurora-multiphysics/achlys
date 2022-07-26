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

class ADAdsorbingMaterial;

// template <>
// InputParameters validParams<ADAdsorbingMaterial>();

class ADAdsorbingMaterial : public ADMaterial
{
public:
  static InputParameters validParams();
  ADAdsorbingMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

private:

  //std::vector<const VariableValue *> _simulation_temperature;
  Real _v0d;
  Real _v0sb;
  Real _v0bs;
  Real _Er;
  Real _Ed;
  Real _Ea;
  Real _k_boltz;
  Real _lambda;
  // Real _lambda_abs;
  // Real _lambda_des;
  Real _n_sol;
  // Real _n_surf_val;
  Real _rho;
  Real _mass;
  Real _sigma;
  Real _s;



  const ADVariableValue & _T;
  const ADVariableValue & _P;

  ADMaterialProperty<Real> & _Vd;
  ADMaterialProperty<Real> & _Vsb;
  ADMaterialProperty<Real> & _Vbs;
  ADMaterialProperty<Real> & _Gamma;
  ADMaterialProperty<Real> & _Sigma;
  ADMaterialProperty<Real> & _n_surf;
  ADMaterialProperty<Real> & _S;
  ADMaterialProperty<Real> & _Lambda_abs;


};