// This file is part of the MOOSE framework
// https://www.mooseframework.org
//
// All rights reserved, see COPYRIGHT for full restrictions
// https://github.com/idaholab/moose/blob/master/COPYRIGHT
//
// Licensed under LGPL 2.1, please see LICENSE for details
// https://www.gnu.org/licenses/lgpl-2.1.html

#include "ExtrinsicStaticTrappingMaterial.h"
#include <cstdio>
#include "Function.h"

registerMooseObject("achlysApp", ExtrinsicStaticTrappingMaterial);

InputParameters ExtrinsicStaticTrappingMaterial::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addRequiredParam<Real>("v0", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<Real>("E1", "Trap detrapping energy in eV");
  params.addRequiredParam<Real>("E2", "Trap detrapping energy in eV");
  params.addRequiredParam<Real>("E3", "Trap detrapping energy in eV");
  params.addRequiredParam<Real>("k_boltz", "Boltzman constant");
  params.addRequiredParam<Real>("D0", "The diffusion pre-exponential factor");
  params.addRequiredParam<Real>("E_diff", "diffusion energy in eV");
  params.addRequiredParam<Real>("lambda", "Lattice constant in m-1");
  params.addRequiredParam<Real>("n_sol", "density of interstitial sites");
  params.addRequiredParam<Real>("rho", "material density in m^-3");
  params.addRequiredParam<Real>("n1", "possible trapping sites");
  params.addRequiredParam<Real>("n2", "possible trapping sites");
//   params.addRequiredParam<Real>("n3", "possible trapping sites");
  params.addRequiredParam<Real>("n3a_max", "Maximum trap density - \"a\" type");
  params.addRequiredParam<Real>("n3b_max", "Maximum attainted trap density - \"b\" type");
  params.addRequiredParam<Real>("eta_a", "Trap creation rate - \"a\" type");
  params.addRequiredParam<Real>("eta_b", "Trap creation rate - \"b\" type");
  params.addRequiredParam<Real>("flux", "Scaled implantation flux");
  // params.addRequiredParam<Real>("n3ab_transition", "x-position of 50% a-b transition point [nm]");
  //   params.addRequiredParam<Real>("sigmoid_transition_rate_factor", "Muliplication factor in sigmoid exponent");
  params.addRequiredParam<Real>("conductivity", "Thermal conductivity in W/K");
  params.addRequiredParam<Real>("Cp", "Specific heat in J/kg/K");
  params.addRequiredParam<Real>("density", "Material density in kg/m3");
  params.addRequiredParam<Real>("const_T", "placeholder for temperature");
  params.addRequiredParam<Real>("xp", "region where trap3 exists");
  params.addRequiredParam<Real>("trap_evolution_time", "Previous flux exposure time for trap distribution calculation");
  // params.addCoupledVar("Temperature","simulation temperature");
  params.addParam<FunctionName>("function", 0, "The function describing the implantation flux distribution");
  // could add optional temperature function..
  // params.addParam<FunctionName>("function", 0, "The function describing the implantation flux distribution");
  return params;
}

ExtrinsicStaticTrappingMaterial::ExtrinsicStaticTrappingMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    //_simulation_temperature(coupledValues("Temperature")),
    _v0(getParam<Real>("v0")),
    _E1(getParam<Real>("E1")),
    _E2(getParam<Real>("E2")),
    _E3(getParam<Real>("E3")),
    _k_boltz(getParam<Real>("k_boltz")),
    _D0(getParam<Real>("D0")),
    _E_diff(getParam<Real>("E_diff")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _rho(getParam<Real>("rho")),
    _const_sites_avail_type_1(getParam<Real>("n1")),
    _const_sites_avail_type_2(getParam<Real>("n2")),
    _n3a(getParam<Real>("n3a_max")),
    _n3b(getParam<Real>("n3b_max")),
    _eta_a(getParam<Real>("eta_a")),
    _eta_b(getParam<Real>("eta_b")),
    _flux(getParam<Real>("flux")),
    // _n3ab_x(getParam<Real>("n3ab_transition")),
    // _sigmoid_sf(getParam<Real>("sigmoid_transition_rate_factor")),
    _conductivity(getParam<Real>("conductivity")),
    _cp(getParam<Real>("Cp")),
    _density(getParam<Real>("density")),
    _T(getParam<Real>("const_T")),
    _xp(getParam<Real>("xp")),
    _user_defined_time(getParam<Real>("trap_evolution_time")),
    // _T(adCoupledValue("Temperature")),
    _flux_function(getFunction("function")),
    _D(declareADProperty<Real>("D")),
    _Vm(declareADProperty<Real>("Vm")),
    _V1(declareADProperty<Real>("V1")),
    _V2(declareADProperty<Real>("V2")),
    _V3(declareADProperty<Real>("V3")),
    _n1(declareADProperty<Real>("n1")),
    _n2(declareADProperty<Real>("n2")),
    _n3(declareADProperty<Real>("n3")),
    _Thermal_diffusivity(declareADProperty<Real>("thermal_diffusivity")),
    _Conductivity(declareADProperty<Real>("conductivity")),
    _Density(declareADProperty<Real>("density")),
    _Cp(declareADProperty<Real>("Cp"))
    
{
  _theta = 1.0 / _xp;
}

void
ExtrinsicStaticTrappingMaterial::computeQpProperties()
{

_D[_qp] = _D0 * std::exp((-1.0 * _E_diff) / (_k_boltz * _T));
_Vm[_qp] = _rho * _D[_qp] / ( std::pow(_lambda, 2) * _n_sol);
_V1[_qp] = _v0 * std::exp((-1.0 * _E1) / (_k_boltz * _T));
_V2[_qp] = _v0 * std::exp((-1.0 * _E2) / (_k_boltz * _T));
_V3[_qp] = _v0 * std::exp((-1.0 * _E3) / (_k_boltz * _T));
_n1[_qp] = _const_sites_avail_type_1;
_n2[_qp] = _const_sites_avail_type_2;
if (_n3a > 0.0){
  if(_q_point[_qp](0)< _xp){
    Real fx = _flux_function.value(_t, _q_point[_qp]);
    Real combined_n3_max = _n3a * _n3b * ( (fx * _eta_a) + (_theta * _eta_b) ) 
                        / ( ( _n3a * _theta * _eta_b) + (_n3b * fx * _eta_a) );
    Real combined_rates = (fx * _eta_a / _n3a) + (_theta * _eta_b / _n3b);
    Real N3_exponent = -1.0 * _flux * combined_rates * _user_defined_time;
    _n3[_qp] = combined_n3_max * ( 1 - std::exp(N3_exponent) );
  // } else if (_q_point[_qp](0)< 1.0 *_xp) { // boundary smoothing...
  //   _n3[_qp] = (1 - (1.0 / (0.1*_xp) * ( _q_point[_qp](0) - 0.9* _xp))) * _n3b;
  } else {
    _n3[_qp] = 0.0;
}
}

_Thermal_diffusivity[_qp] = _conductivity / (_density * _cp);
_Conductivity[_qp] = _conductivity;
_Density[_qp] = _density;
_Cp[_qp] = _cp;


}