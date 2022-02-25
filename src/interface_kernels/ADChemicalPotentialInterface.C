#include "ADChemicalPotentialInterface2.h"

registerMooseObject("achlysApp", ADChemicalPotentialInterface2);

InputParameters
ADChemicalPotentialInterface2::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("s", "s", "The Solubility of primary variable concentration in this material");
  params.addParam<MaterialPropertyName>("s_neighbour", "s", "The Solubility of primary variable concentration in the neighbouring material");
  // params.addParam<Real>("penalty", 1e6, "Penalty term");
  params.addClassDescription(
      "The kernel is utilized to establish equivalence of chemical potential on an interface for variables.");
  return params;
}

ADChemicalPotentialInterface2::ADChemicalPotentialInterface2(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _s(getADMaterialProperty<Real>("s")),
  _s_neighbour(getNeighborADMaterialProperty<Real>("s_neighbour"))
  // _p(getParam<Real>("penalty"))
{
}

ADReal
ADChemicalPotentialInterface2::computeQpResidual(Moose::DGResidualType type)
{
   ADReal r = 0; 

  switch (type)
  {
    case Moose::Element:
      r = _u[_qp] - (_neighbor_value[_qp] *_s[_qp] / _s_neighbour[_qp]);
      return r;


    case Moose::Neighbor:
      r = _neighbor_value[_qp] - (_u[_qp] * _s_neighbour[_qp]/ _s[_qp]);
      return r;
  }

  mooseError("Internal error.");
}
