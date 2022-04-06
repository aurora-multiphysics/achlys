#include "ADMatInterfaceDiffusion.h"

registerMooseObject("achlysApp", ADMatInterfaceDiffusion);

InputParameters
ADMatInterfaceDiffusion::validParams()
{
  InputParameters params = ADInterfaceKernel::validParams();
  params.addClassDescription(
      "Enforce flux continuity between two different variables across a subdomain boundary");
  params.addParam<MaterialPropertyName>("D",
                    "D",
                    "the name of the diffusion constant material property");
  params.addParam<MaterialPropertyName>("D_neighbour",
                    "D",
                    "the name of the neighbouring diffusion constant material property");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addParam<MaterialPropertyName>("rho_neighbour",
                    "rho_neighbour",
                    "the name of the lattice density material property");
  return params;
}

ADMatInterfaceDiffusion::ADMatInterfaceDiffusion(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _D(getADMaterialProperty<Real>("D")),
  _D_neighbor(getNeighborADMaterialProperty<Real>("D_neighbour")),
  _rho(getADMaterialProperty<Real>("rho")),
  _rho_neighbor(getNeighborADMaterialProperty<Real>("rho_neighbour"))
{
}

ADReal
ADMatInterfaceDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  // equal gradients means difference is zero
  ADReal res =
      (_rho[_qp] * _D[_qp] * _grad_u[_qp] * _normals[_qp])
       - (_rho_neighbor[_qp] * _D_neighbor[_qp] * _grad_neighbor_value[_qp] * _normals[_qp]);

  switch (type)
  {
    case Moose::Element:
      return (res / _rho[_qp]) * _test[_i][_qp]; // * -1?

    case Moose::Neighbor:
      return (res / _rho_neighbor[_qp]) * _test_neighbor[_i][_qp];
  }

  mooseError("Internal error.");
}