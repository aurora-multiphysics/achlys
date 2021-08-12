//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "DirichletEquivalentPfcImplantBC.h"

registerMooseObject("achlysApp", DirichletEquivalentPfcImplantBC);

InputParameters
DirichletEquivalentPfcImplantBC::validParams()
{
  InputParameters params = ADDirichletBCBase::validParams();
  params.addRequiredCoupledVar("T", "Temperature");
  params.addParam<Real>("flux", 0.0, "The value of the implantation flux.");
  params.addRequiredParam<Real>("k0","pre-exponent factor");
  params.addRequiredParam<Real>("E","Energy Barrier in Arhhenious Expression");
  params.addRequiredParam<Real>("scale_factor","scale factor");
  params.addClassDescription("Imposes the essential boundary condition $u=g$, where $g$ "
                             "is a constant calculated assuming the recombination flux"
                             "quickly becomes equal to the incident flux. The recombination"
                             " term is calculated using the simulation temperature and the "
                             "pre-exponent and Energy values provided");
  
  return params;
}

DirichletEquivalentPfcImplantBC::DirichletEquivalentPfcImplantBC(const InputParameters & parameters)
  : ADDirichletBCBase(parameters),
    _T(coupledValue("T")),
    _flux(getParam<Real>("flux")),
    _K0(getParam<Real>("k0")),
    _E(getParam<Real>("E")),
    _scale_factor(getParam<Real>("scale_factor"))
{
}

ADReal
DirichletEquivalentPfcImplantBC::computeQpValue()
{
  Real K = _K0 * std::exp((-1.0 * _E) / (8.617333E-5 * _T[_qp]) );
  ADReal ss_surface_conc = std::sqrt(_flux / K) / _scale_factor;
  return ss_surface_conc;
}
