// This file is part of the MOOSE framework
// https://www.mooseframework.org
//
// All rights reserved, see COPYRIGHT for full restrictions
// https://github.com/idaholab/moose/blob/master/COPYRIGHT
//
// Licensed under LGPL 2.1, please see LICENSE for details
// https://www.gnu.org/licenses/lgpl-2.1.html

#include "VectorTrappingMaterial.h"
#include <cstdio>

registerMooseObject("achlysApp", VectorTrappingMaterial);

template <>
InputParameters
validParams<VectorTrappingMaterial>()
{
  InputParameters params = validParams<ADMaterial>();
  params.addRequiredParam<std::vector<Real>>("v0", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<std::vector<Real>>("Ei", "Trap detrapping energy in eV");
  params.addRequiredParam<Real>("k_boltz", "Boltzman constant");
  params.addRequiredParam<Real>("D0", "The diffusion pre-exponential factor");
  params.addRequiredParam<Real>("E_diff", "diffusion energy in eV");
  params.addRequiredParam<Real>("lambda", "Lattice constant in m-1");
  params.addRequiredParam<Real>("n_sol", "density of interstitial sites");
  params.addRequiredParam<std::vector<Real>>("ni", "possible trapping sites");
  params.addRequiredParam<Real>("conductivity", "Thermal conductivity in W/K");
  params.addRequiredParam<Real>("Cp", "Specific heat in J/kg/K");
  params.addRequiredParam<Real>("density", "Material density in kg/m3");
  params.addCoupledVar("Temperature","simulation temperature");

  return params;
}

VectorTrappingMaterial::VectorTrappingMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _v0(getParam<std::vector<Real>>("v0")),
    _Ei(getParam<std::vector<Real>>("Ei")),
    _k_boltz(getParam<Real>("k_boltz")),
    _D0(getParam<Real>("D0")),
    _E_diff(getParam<Real>("E_diff")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _ni(getParam<std::vector<Real>>("ni")),
    _conductivity(getParam<Real>("conductivity")),
    _cp(getParam<Real>("Cp")),
    _density(getParam<Real>("density")),
    _T(adCoupledValue("Temperature")),
    _D(declareADProperty<Real>("D")),
    _Vm(declareADProperty<Real>("Vm")),
    _Vi(declareADProperty<std::vector<Real>>("V0")),
    _n(declareADProperty<std::vector<Real>>("n")),
    _Thermal_diffusivity(declareADProperty<Real>("thermal_diffusivity")),
    _Conductivity(declareADProperty<Real>("conductivity")),
    _Density(declareADProperty<Real>("density")),
    _Cp(declareADProperty<Real>("Cp"))
    

{
}

ADReal VectorTrappingMaterial::Arhhenious(Real v0, Real E)
{
  return v0 * std::exp((-1.0 * E) / (_k_boltz * _T[_qp]));
}

void
VectorTrappingMaterial::computeQpProperties()
{
  // test vectors are all same length
  bool equal_arrays = _v0.size() == _ni.size() && _Ei.size() == _ni.size();
  if (! equal_arrays)
  {
    mooseError("The trap parameter vectors v0, ni, and Ei must be the same length");
  }

_D[_qp] = _D0 * std::exp((-1.0 * _E_diff) / (_k_boltz * _T[_qp]));
_Vm[_qp] = _D[_qp] / ( std::pow(_lambda, 2) * _n_sol);


for(const auto& value: _ni)
{
  _n[_qp].push_back(value);
}

for (int i = 0; i < _v0.size(); i++)
{
  _Vi[_qp].push_back(
    _v0[i]* std::exp((-1.0 * _Ei[i]) / (8.6e-5 * _T[_qp])) 
    );
}

// // apply arhennious function to each value in a pair of vectors.
// std::transform(_v0.begin(), _v0.end(), _Ei.begin(), _V0[_qp], 
// [] (Real i, Real j) -> Real {return Arhhenious(i, j, _T[_qp]);});

// std::transform(_v0.begin(), _v0.end(), _Ei.begin(), _Vi[_qp], Arhhenious);

// std::transform(
//   _v0.begin(), _v0.end(), _Ei.begin(),
//   _Vi[_qp], 
//   [] (Real i, Real j) {return Arhhenious(i,j);}
// );


_Thermal_diffusivity[_qp] = _conductivity / (_density * _cp);
_Conductivity[_qp] = _conductivity;
_Density[_qp] = _density;
_Cp[_qp] = _cp;

}