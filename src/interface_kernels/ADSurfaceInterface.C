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
  params.addRequiredParam<Real>("n_surf", "The density of surface sites");
  params.addRequiredParam<Real>("lambda", "lambda");
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
    _n_surf(getParam<Real>("n_surf")),
    _lambda(getParam<Real>("lambda"))
{
}

ADReal
ADSurfaceInterface::computeQpResidual(Moose::DGResidualType type)
{
  ADReal r = 0;
  // ADReal theta = _u[_qp] / _n_surf * 6.3e28;
  ADReal flux_in = _vsb[_qp] * _u[_qp];
  ADReal flux_out = _vbs[_qp] * _neighbor_value[_qp] * (1.0 - _u[_qp]) * 6.3e28;
  // ADReal surface_flux_in = 2* _Gamma[_qp] * _S[_qp];
  // ADReal surface_flux_out = _vd[_qp] * _u[_qp] * _u[_qp];
  ADReal diffusive_flux = _D_neighbor[_qp] *  _grad_neighbor_value[_qp] * _normals[_qp] * 6.3e28;

  // if (_neighbor_value[_qp] < 0.0)
  // {
  //   flux_out = 0.0;
  // }


  // flux_out = 0.0;
  // if (_u[_qp] >= 1.0)
  // {
  //   surface_flux_in = 0.0;
  // }

  // if (_u[_qp] < 0.0)
  // {
  //   surface_flux_out = 0.0;
  // }

  // ADReal surface_flux = surface_flux_in - surface_flux_out;

  switch (type)
  {
    case Moose::Element:
      r =  -1.0 *_test[_i][_qp] * (flux_out - flux_in);
      break;

    case Moose::Neighbor:
      r = _test_neighbor[_i][_qp]  * -1.0 *  (flux_in - flux_out) * _n_surf / (_lambda * 6.3e28);
      break;
  }

  return r;
}

// Real
// InterfaceDiffusion::computeQpJacobian(Moose::DGJacobianType type)
// {
//   Real jac = 0;

//   switch (type)
//   {
//     case Moose::ElementElement:
//     case Moose::NeighborNeighbor:
//       break;

//     case Moose::NeighborElement:
//       jac = _test_neighbor[_i][_qp] * _D[_qp] * _grad_phi[_j][_qp] * _normals[_qp];
//       break;

//     case Moose::ElementNeighbor:
//       jac = _test[_i][_qp] * -_D_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp];
//       break;
//   }

//   return jac;
// }