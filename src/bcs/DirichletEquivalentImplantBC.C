//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "DirichletEquivalentImplantBC.h"

registerMooseObject("achlysApp", DirichletEquivalentImplantBC);

InputParameters
DirichletEquivalentImplantBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  // InputParameters params = ADDirichletBCBase::validParams();
  params.addParam<Real>("flux", 0.0, "The value of the implantation flux.");
  params.addRequiredParam<Real>("R","The implantation depth in m");
  params.addParam<MaterialPropertyName>("D",
                    "D",
                    "the name of the diffusivity material property");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addClassDescription("Imposes the essential boundary condition $u=g$, where $g$ "
                             "is a constant calculated assuming the recombination flux"
                             "quickly becomes equal to the incident flux. The recombination"
                             " term is calculated using the simulation temperature and the "
                             "pre-exponent and Energy values provided");
  
  return params;
}
// Moose::EMPTY_BLOCK_IDS, blockIDs()
 // : ADDirichletBCBase(parameters), MaterialPropertyInterface(this, Moose::EMPTY_BLOCK_IDS, boundaryIDs()),
DirichletEquivalentImplantBC::DirichletEquivalentImplantBC(const InputParameters & parameters)
    : ADIntegratedBC(parameters),
    _flux(getParam<Real>("flux")),
    _R(getParam<Real>("R")),
    _D(getADMaterialProperty<Real>("D")),
    _rho(getADMaterialProperty<Real>("rho"))
{
}

// DirichletEquivalentImplantBC::computeQpValue()
ADReal
DirichletEquivalentImplantBC::computeQpResidual()
{
  ADReal surface_concentration = _flux * _R / (_D[_qp] * _rho[_qp]);
  return _u[_qp] - surface_concentration;
}
