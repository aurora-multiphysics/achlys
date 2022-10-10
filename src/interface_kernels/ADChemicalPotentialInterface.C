#include "ADChemicalPotentialInterface.h"

registerMooseObject("achlysApp", ADChemicalPotentialInterface);

InputParameters
ADChemicalPotentialInterface::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("s", "s", "The Solubility of primary variable concentration in this material");
  params.addParam<MaterialPropertyName>("neighbor_s", "s", "The Solubility of primary variable concentration in the neighbouring material");
  params.addClassDescription(
      "The kernel is utilized to establish equivalence of chemical potential on an interface for variables.");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addParam<MaterialPropertyName>("neighbor_rho",
                    "rho",
                    "the name of the lattice density material property");
  return params;
}

ADChemicalPotentialInterface::ADChemicalPotentialInterface(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _s(getADMaterialProperty<Real>("s")),
  _s2(getNeighborADMaterialProperty<Real>("neighbor_s")),
  _rho(getADMaterialProperty<Real>("rho")),
  _rho2(getNeighborADMaterialProperty<Real>("neighbor_rho"))
{
}

ADReal
ADChemicalPotentialInterface::computeQpResidual(Moose::DGResidualType type)
{
   ADReal r = 0; 

  switch (type)
  {
    case Moose::Element:
      r = _u[_qp] - ( (_rho2[_qp] / _rho[_qp]) * (_neighbor_value[_qp] *_s[_qp] / _s2[_qp]) );
      r *= _test[_i][_qp];
      return r;


    case Moose::Neighbor:
      r = _neighbor_value[_qp] - ( (_rho[_qp] / _rho2[_qp]) * (_u[_qp] * _s2[_qp]/ _s[_qp]) );
      r*= _test_neighbor[_i][_qp];
      return r;
  }

  mooseError("Internal error.");
}
