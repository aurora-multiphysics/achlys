//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PfcFunctionFluxRampBC.h"
#include "Function.h"

registerMooseObject("achlysApp", PfcFunctionFluxRampBC);

InputParameters
PfcFunctionFluxRampBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredCoupledVar("T", "Temperature");
  params.addRequiredParam<Real>("k0","pre-exponent factor");
  params.addRequiredParam<Real>("E","Energy Barrier in Arhhenious Expression");
  params.addRequiredParam<Real>("scale_factor","scale factor");
  params.addClassDescription("Imposes the essential boundary condition $u=g$, where $g$ "
                             "is a constant calculated assuming the recombination flux"
                             "quickly becomes equal to the incident flux. The recombination"
                             " term is calculated using the simulation temperature and the "
                             "pre-exponent and Energy values provided");
  params.addParam<FunctionName>("function", 0, "The function describing the Dirichlet condition");
  
  return params;
}

PfcFunctionFluxRampBC::PfcFunctionFluxRampBC(const InputParameters & parameters)
    : ADIntegratedBC(parameters),
    _T(coupledValue("T")),
    _K0(getParam<Real>("k0")),
    _E(getParam<Real>("E")),
    _scale_factor(getParam<Real>("scale_factor")),
    _flux_function(getFunction("function"))
{
}

ADReal
PfcFunctionFluxRampBC::computeQpResidual()
{
  Real K = _K0 * std::exp((-1.0 * _E) / (8.617333E-5 * _T[_qp]) );
  Real flux_value = _flux_function.value(_t, _q_point[_qp]) / _scale_factor;
  ADReal desorption_flux = K * _scale_factor * _u[_qp] * _u[_qp];
  return  -1.0 * _test[_i][_qp] * (flux_value - desorption_flux);
}
