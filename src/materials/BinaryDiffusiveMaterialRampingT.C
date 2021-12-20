
#include "BinaryDiffusiveMaterialRampingT.h"
#include <cstdio>

registerMooseObject("achlysApp", BinaryDiffusiveMaterialRampingT);

template <>
InputParameters
validParams<BinaryDiffusiveMaterialRampingT>()
{
  InputParameters params = validParams<ADMaterial>();
  params.addRequiredParam<Real>("k_boltz", "Boltzman constant");
  params.addRequiredParam<Real>("D0_1", "The diffusion pre-exponential factor");
  params.addRequiredParam<Real>("E_diff_1", "diffusion energy in eV");
  params.addRequiredParam<Real>("D0_2", "The diffusion pre-exponential factor");
  params.addRequiredParam<Real>("E_diff_2", "diffusion energy in eV");
  params.addRequiredParam<Real>("lambda", "Lattice constant in m-1");
  params.addRequiredParam<Real>("n_sol", "density of interstitial sites");
  params.addRequiredParam<Real>("T0", "Initial Temperature in Kelvin");
  params.addRequiredParam<Real>("beta", "Temperature ramp rate in K/s");
  
  // params.addCoupledVar("Temperature","simulation temperature");

  return params;
}

BinaryDiffusiveMaterialRampingT::BinaryDiffusiveMaterialRampingT(const InputParameters & parameters)
  : ADMaterial(parameters),
    _k_boltz(getParam<Real>("k_boltz")),
    _D0_1(getParam<Real>("D0_1")),
    _E_diff_1(getParam<Real>("E_diff_1")),
    _D0_2(getParam<Real>("D0_2")),
    _E_diff_2(getParam<Real>("E_diff_2")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _T0(getParam<Real>("T0")),
    _beta(getParam<Real>("beta")),
    // _T(adCoupledValue("Temperature")),
    _D1(declareADProperty<Real>("D1")),
    _D2(declareADProperty<Real>("D2")),
    _Vm1(declareADProperty<Real>("Vm1")),
    _Vm2(declareADProperty<Real>("Vm2"))  
{
}

void
BinaryDiffusiveMaterialRampingT::computeQpProperties()
{
Real T = _T0 + (_beta * _t);
_D1[_qp] = _D0_1 * std::exp((-1.0 * _E_diff_1) / (_k_boltz * T));
_D2[_qp] = _D0_2 * std::exp((-1.0 * _E_diff_2) / (_k_boltz * T));
_Vm1[_qp] = _D1[_qp] / ( std::pow(_lambda, 2) * _n_sol);
_Vm2[_qp] = _D2[_qp] / ( std::pow(_lambda, 2) * _n_sol);
}