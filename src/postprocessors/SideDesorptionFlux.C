//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SideDesorptionFlux.h"

// #include "metaphysicl/raw_type.h"

registerMooseObject("achlysApp", SideDesorptionFlux);

// defineLegacyParams(SideFluxIntegral);

InputParameters
SideDesorptionFlux::validParams()
{
  InputParameters params = SideIntegralVariablePostprocessor::validParams();
  params.addClassDescription("Computes the integral of the flux over the specified boundary");
    params.addParam<Real>(
      "scale_factor", 1.0, "Coefficent ($\\sigma$) multiplier for the coupled force term.");
  params.addRequiredCoupledVar("Temperature", "Temperature");
  params.addRequiredParam<Real>("k0","pre-exponent factor");
  params.addRequiredParam<Real>("E","Energy Barrier in Arhhenious Expression");
  params.addCoupledVar("variable","Variable name for outflow calculation.");
  return params;
  
  return params;
}

SideDesorptionFlux::SideDesorptionFlux(const InputParameters & parameters)
  : SideIntegralVariablePostprocessor(parameters),
    _T(coupledValue("Temperature")),
    _K0(getParam<Real>("k0")),
    _E(getParam<Real>("E")),
    _scale_factor(getParam<Real>("scale_factor")),
    _variable(adCoupledValue("variable"))
{
}

Real
SideDesorptionFlux::computeQpIntegral()
{
  Real K = _K0 * std::exp((-1.0 * _E) / (8.617333E-5 * _T[_qp]) );
  Real scaled_c_squared = _scale_factor * MetaPhysicL::raw_value(_variable[_qp] * _variable[_qp]);
  return K * scaled_c_squared;
  // return -MetaPhysicL::raw_value(_diffusion_coef[_qp]) * _grad_u[_qp] * _normals[_qp];
}
