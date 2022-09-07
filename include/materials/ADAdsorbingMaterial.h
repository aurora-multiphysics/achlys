#pragma once

#include "ADMaterial.h"
#include "LinearInterpolation.h"

class ADAdsorbingMaterial;

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
