#include "ADVariableMatch.h"

registerMooseObject("achlysApp", ADVariableMatch);

InputParameters
ADVariableMatch::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  // params.addParam<MaterialPropertyName>("s", "s", "The Solubility of primary variable concentration in this material");
  // params.addParam<MaterialPropertyName>("s_neighbour", "s", "The Solubility of primary variable concentration in the neighbouring material");
  // params.addParam<Real>("penalty", 1e6, "Penalty term");
  params.addClassDescription(
      "The kernel is utilized to establish equivalence of variable values on an interface for variables.");
  return params;
}

ADVariableMatch::ADVariableMatch(const InputParameters & parameters)
  : ADInterfaceKernel(parameters)
{
}

ADReal
ADVariableMatch::computeQpResidual(Moose::DGResidualType type)
{
   ADReal r = 0; 

  switch (type)
  {
    case Moose::Element:
      r = (_u[_qp] - _neighbor_value[_qp]); //* _test[_i][_qp];
      return r;


    case Moose::Neighbor:
      r = (_neighbor_value[_qp] - _u[_qp]); //* _test_neighbor[_i][_qp];
      return r;
  }

  mooseError("Internal error.");
}
