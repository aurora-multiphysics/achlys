#include "ADVariableMatch.h"

registerMooseObject("achlysApp", ADVariableMatch);

InputParameters
ADVariableMatch::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("rho", "s", "The lattice density of this material");
  params.addParam<MaterialPropertyName>("rho_neighbor", "s", "The lattice density in the neighboring material");
  // params.addParam<Real>("penalty", 1e6, "Penalty term");
  params.addClassDescription(
      "The kernel is utilized to establish equivalence of variable values on an interface for variables.");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addParam<MaterialPropertyName>("rho_neighbor",
                    "rho_neighbor",
                    "the name of the lattice density material property");
  return params;
}

ADVariableMatch::ADVariableMatch(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
    _rho(getADMaterialProperty<Real>("rho")),
    _rho_neighbor(getNeighborADMaterialProperty<Real>("rho_neighbor"))
{
}

ADReal
ADVariableMatch::computeQpResidual(Moose::DGResidualType type)
{
   ADReal r = 0; 

  switch (type)
  {
    case Moose::Element:
      r = ( (_u[_qp] * _rho[_qp])  - (_neighbor_value[_qp] * _rho_neighbor[_qp]) ) / _rho[_qp]; //* _test[_i][_qp];
      return r;


    case Moose::Neighbor:
      r = ( (_rho_neighbor[_qp]*_neighbor_value[_qp]) - (_rho[_qp]*_u[_qp]) ) / _rho_neighbor[_qp]; //* _test_neighbor[_i][_qp];
      return r;
  }

  mooseError("Internal error.");
}
