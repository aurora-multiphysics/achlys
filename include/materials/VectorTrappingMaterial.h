#pragma once

#include "ADMaterial.h"
#include "LinearInterpolation.h"

class VectorTrappingMaterial;

class VectorTrappingMaterial : public ADMaterial
{
public:
  static InputParameters validParams();
  VectorTrappingMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  // virtual void initQpStatefulProperties() override;
  ADReal Arhhenious(Real v0, Real E);

private:

  //std::vector<const VariableValue *> _simulation_temperature;
  const std::vector<Real> _v0;
  const std::vector<Real> _Ei;
  const std::vector<Real> _ni;
  std::vector<std::string> _ni_names;
  std::vector<std::string> _vi_names;

  Real _k_boltz;
  Real _D0;
  Real _E_diff;
  Real _lambda;
  Real _n_sol;
  // RealEigenVector _ni;
  

  const ADVariableValue & _T;
  ADMaterialProperty<Real> & _D;
  ADMaterialProperty<Real> & _Vm;

  int _num_traps;
  std::vector<ADMaterialProperty<Real> *> _n;
  std::vector<ADMaterialProperty<Real> *> _Vi;
};
