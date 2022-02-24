#include "ADChemicalPotentialInterface.h"

registerMooseObject("achlysApp", ADChemicalPotentialInterface);

InputParameters
ADChemicalPotentialInterface::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("s", "s", "The Solubility of primary variable concentration in this material");
  params.addParam<MaterialPropertyName>("s_neighbour", "s", "The Solubility of primary variable concentration in the neighbouring material");
  params.addParam<Real>("penalty", 1e6, "Penalty term");
  params.addClassDescription(
      "The kernel is utilized to establish equivalence of chemical potential on an interface for variables.");
  return params;
}

ADChemicalPotentialInterface::ADChemicalPotentialInterface(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _s(getADMaterialProperty<Real>("s")),
  _s_neighbour(getNeighborADMaterialProperty<Real>("s_neighbour")),
  _p(getParam<Real>("penalty"))
{
}

ADReal
ADChemicalPotentialInterface::computeQpResidual(Moose::DGResidualType type)
{
  ADReal r = (_u[_qp] / _s[_qp]) - (_neighbor_value[_qp] / _s_neighbour[_qp]);

  switch (type)
  {
    case Moose::Element:
      r *= 1.0 *_test[_i][_qp] * _p; 

      break;

    case Moose::Neighbor:
      r *=  -1.0  *_test_neighbor[_i][_qp] * _p;
      break;
  }

  return r;
}
