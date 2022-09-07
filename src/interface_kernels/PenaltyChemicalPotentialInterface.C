#include "PenaltyChemicalPotentialInterface.h"

registerMooseObject("achlysApp", PenaltyChemicalPotentialInterface);

InputParameters
PenaltyChemicalPotentialInterface::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("s", "s", "The Solubility of primary variable concentration in this material");
  params.addParam<MaterialPropertyName>("s_neighbor", "s", "The Solubility of primary variable concentration in the neighboring material");
  params.addParam<Real>("penalty", 1e6, "Penalty term");
  params.addClassDescription(
      "The kernel is utilized to establish equivalence of chemical potential on an interface for variables.");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addParam<MaterialPropertyName>("rho_neighbor",
                    "rho_neighbor",
                    "the name of the lattice density material property");
  return params;
}

PenaltyChemicalPotentialInterface::PenaltyChemicalPotentialInterface(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _s(getADMaterialProperty<Real>("s")),
  _s_neighbor(getNeighborADMaterialProperty<Real>("s_neighbor")),
  _rho(getADMaterialProperty<Real>("rho")),
  _rho_neighbor(getNeighborADMaterialProperty<Real>("rho_neighbor")),
  _p(getParam<Real>("penalty"))
{
}

ADReal
PenaltyChemicalPotentialInterface::computeQpResidual(Moose::DGResidualType type)
{
  ADReal r = (_u[_qp] * _rho[_qp]/ _s[_qp])
    - (_neighbor_value[_qp] *_rho_neighbor[_qp]/ _s_neighbor[_qp]);

  switch (type)
  {
    case Moose::Element:
      r/= _rho[_qp];
      r *= 1.0 *_test[_i][_qp] * _p; 

      break;

    case Moose::Neighbor:
      r/= _rho_neighbor[_qp];
      r *=  -1.0  *_test_neighbor[_i][_qp] * _p;
      break;
  }

  return r;
}
