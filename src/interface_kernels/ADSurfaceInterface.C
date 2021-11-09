// This file is part of the MOOSE framework
// https://www.mooseframework.org
//
// All rights reserved, see COPYRIGHT for full restrictions
// https://github.com/idaholab/moose/blob/master/COPYRIGHT
//
// Licensed under LGPL 2.1, please see LICENSE for details
// https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADSurfaceInterface.h"

registerMooseObject("achlysApp", ADSurfaceInterface);

InputParameters
ADSurfaceInterface::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("vd", "vd", "The desorption rate");
  params.addParam<MaterialPropertyName>("vsb", "vsb", "The forward rate for transport between surface and bulk");
  params.addParam<MaterialPropertyName>("vbs", "vbs", "The revese rate for transport from the bulk to surface");
  params.addParam<MaterialPropertyName>("sigma", "sigma", "The cross section for recombination of an incoming particle with one trapped on the surface");
  params.addParam<MaterialPropertyName>("S", "S", "The sticking probability for the incident particle flux");
  params.addParam<MaterialPropertyName>("Gamma", "Gamma", "The incident particle flux");
  params.addParam<MaterialPropertyName>(
      "D_neighbor", "D_neighbor", "The neighboring diffusion coefficient.");
  params.addClassDescription(
      "The kernel is utilized to establish flux equivalence on an interface for variables.");
  // params.addRequiredParam<Real>("n_surf", "The density of surface sites");
  params.addParam<MaterialPropertyName>("n_surf", "n_surf", "Desorption rate");
  params.addParam<MaterialPropertyName>("lambda_abs", "Lambda_abs", "Material property for recirpical square root of surface trap site concentration");
  return params;
}

ADSurfaceInterface::ADSurfaceInterface(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
    _vd(getADMaterialProperty<Real>("vd")),
    _vsb(getADMaterialProperty<Real>("vsb")),
    _vbs(getADMaterialProperty<Real>("vbs")),
    _sigma(getADMaterialProperty<Real>("sigma")),
    _S(getADMaterialProperty<Real>("S")),
    _Gamma(getADMaterialProperty<Real>("Gamma")),
    _D_neighbor(getNeighborADMaterialProperty<Real>("D_neighbor")),
    _n_surf(getADMaterialProperty<Real>("n_surf")),
    _lambda(getADMaterialProperty<Real>("Lambda_abs"))
{
}

ADReal
ADSurfaceInterface::computeQpResidual(Moose::DGResidualType type)
{
  ADReal r = 0;

  ADReal flux_in = _vsb[_qp] * _u[_qp];
  ADReal flux_out = _vbs[_qp] * _neighbor_value[_qp] * (1.0 - _u[_qp]) * 6.3e28;

  ADReal diffusive_flux = _D_neighbor[_qp] *  _grad_neighbor_value[_qp] * _normals[_qp] * 6.3e28;


  switch (type)
  {
    case Moose::Element:
      r =  -1.0 *_test[_i][_qp] * (flux_out - flux_in);
      break;

    case Moose::Neighbor:
      r = _test_neighbor[_i][_qp]  * -1.0 *  (flux_in - flux_out) * _n_surf[_qp] / (_lambda[_qp] * 6.3e28);
      break;
  }

  return r;
}
