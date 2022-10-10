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
  params.addParam<MaterialPropertyName>("neighbor_D",
                    "D",
                    "the name of the neighbouring diffusion constant material property");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addParam<MaterialPropertyName>("neighbor_rho",
                    "rho",
                    "the name of the lattice density material property");
  return params;
}

ADMatInterfaceDiffusion::ADMatInterfaceDiffusion(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _D(getADMaterialProperty<Real>("D")),
  _D2(getNeighborADMaterialProperty<Real>("neighbor_D")),
  _rho(getADMaterialProperty<Real>("rho")),
  _rho2(getNeighborADMaterialProperty<Real>("neighbor_rho"))
{
}

ADReal
ADMatInterfaceDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  // equal gradients means difference is zero
  // ADReal res =
  //     (_rho[_qp] * _D[_qp] * _grad_u[_qp] * _normals[_qp])
  //      - (_rho2[_qp] * _D2[_qp] * _grad_neighbor_value[_qp] * _normals[_qp]);
  ADReal res = 0;

  switch (type)
  {
    case Moose::Element:
      res = (_rho2[_qp] * _D2[_qp] * _grad_neighbor_value[_qp] * _normals[_qp]);
      return -(res / _rho[_qp]) * _test[_i][_qp];

    case Moose::Neighbor:
      res = (_rho[_qp] * _D[_qp] * _grad_u[_qp] * _normals[_qp]);
      return (res / _rho2[_qp]) * _test_neighbor[_i][_qp];
  }

  mooseError("Internal error.");
}