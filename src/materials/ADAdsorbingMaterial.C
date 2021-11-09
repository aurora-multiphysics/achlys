// This file is part of the MOOSE framework
// https://www.mooseframework.org
//
// All rights reserved, see COPYRIGHT for full restrictions
// https://github.com/idaholab/moose/blob/master/COPYRIGHT
//
// Licensed under LGPL 2.1, please see LICENSE for details
// https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADAdsorbingMaterial.h"
#include <cstdio>
#include <cmath>

registerMooseObject("achlysApp", ADAdsorbingMaterial);

template <>
InputParameters
validParams<ADAdsorbingMaterial>()
{
  InputParameters params = validParams<ADMaterial>();
  params.addRequiredParam<Real>("v0_d", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<Real>("v0_sb", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<Real>("v0_bs", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<Real>("E_r", "Energy in eV");
  params.addRequiredParam<Real>("E_d", "Energy in eV");
  params.addRequiredParam<Real>("E_a", "Energy in eV");
  params.addRequiredParam<Real>("k_boltz", "Boltzman constant");
  params.addRequiredParam<Real>("lambda", "Lattice constant in m-1");
  params.addRequiredParam<Real>("lambda_abs", "Lattice constant in m-1");
  params.addRequiredParam<Real>("lambda_des", "Lattice constant in m-1");
  params.addRequiredParam<Real>("n_sol", "density of interstitial sites");
  params.addRequiredParam<Real>("n_surf", "density of interstitial sites");
  // params.addRequiredParam<Real>("rho", "material density in m^-3");
  params.addRequiredParam<Real>("rho", "Number density of bulk ions");
  params.addRequiredParam<Real>("mass", "Mass of incident particle");
  params.addRequiredParam<Real>("sigma", "Cross section for recombination of incident and surface particles");
  params.addRequiredParam<Real>("s", "Sticking probability");
  params.addCoupledVar("Temperature","simulation temperature");
  params.addCoupledVar("Pressure","Pressure");

  return params;
}

ADAdsorbingMaterial::ADAdsorbingMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _v0d(getParam<Real>("v0_d")),
    _v0sb(getParam<Real>("v0_sb")),
    _v0bs(getParam<Real>("v0_bs")),
    _Er(getParam<Real>("E_r")),
    _Ed(getParam<Real>("E_d")),
    _Ea(getParam<Real>("E_a")),
    _k_boltz(getParam<Real>("k_boltz")),
    _lambda(getParam<Real>("lambda")),
    _lambda_abs(getParam<Real>("lambda_abs")),
    _lambda_des(getParam<Real>("lambda_des")),
    _n_sol(getParam<Real>("n_sol")),
    _n_surf_val(getParam<Real>("n_surf")),
    _rho(getParam<Real>("rho")),
    _mass(getParam<Real>("mass")),
    _sigma(getParam<Real>("sigma")),
    _s(getParam<Real>("s")),
    _T(adCoupledValue("Temperature")),
    _P(adCoupledValue("Pressure")),
    _Vd(declareADProperty<Real>("Vd")),
    _Vsb(declareADProperty<Real>("Vsb")),
    _Vbs(declareADProperty<Real>("Vbs")),
    _Gamma(declareADProperty<Real>("Gamma")),
    _Sigma(declareADProperty<Real>("Sigma")),
    _n_surf(declareADProperty<Real>("n_surf")),
    _S(declareADProperty<Real>("S"))
{
}

void
ADAdsorbingMaterial::computeQpProperties()
{
_Vd[_qp] = _v0d * std::pow(_lambda_des, 2) * std::exp((-2.0 * _Ed) / (_k_boltz * _T[_qp])) *_n_surf_val;
_Vsb[_qp] = _v0sb * std::exp((-1.0 * _Ea) / (_k_boltz * _T[_qp]));
_Vbs[_qp] = _v0bs * _lambda_abs * std::exp((-1.0 * _Er) / (_k_boltz * _T[_qp])) / _n_surf_val;
_Gamma[_qp] = _P[_qp] / (std::sqrt(2.0 * M_PI * _k_boltz * _T[_qp] * _mass) * _n_surf_val);
_Sigma[_qp] = _sigma;
_n_surf[_qp] = _n_surf_val;
_S[_qp] = _s;


}