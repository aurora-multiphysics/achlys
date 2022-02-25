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
  params.addParam<MaterialPropertyName>("D_neighbor",
                    "D",
                    "the name of the neighbouring diffusion constant material property");
  return params;
}

ADMatInterfaceDiffusion::ADMatInterfaceDiffusion(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _D(getADMaterialProperty<Real>("D")),
  _D_neighbor(getNeighborADMaterialProperty<Real>("D_neigh"))
{
}

ADReal
ADMatInterfaceDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  // equal gradients means difference is zero
  ADReal res =
      _D[_qp] * _grad_u[_qp] * _normals[_qp] - _D_neighbor[_qp] * _grad_neighbor_value[_qp] * _normals[_qp];

  switch (type)
  {
    case Moose::Element:
      return -res * _test[_i][_qp];

    case Moose::Neighbor:
      return res * _test_neighbor[_i][_qp];
  }

  mooseError("Internal error.");
}