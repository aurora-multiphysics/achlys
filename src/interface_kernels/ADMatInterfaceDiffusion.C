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
                    "the name of the neighboring diffusion constant material property");
  params.addParam<MaterialPropertyName>("rho",
                    "rho",
                    "the name of the lattice density material property");
  params.addParam<MaterialPropertyName>("rho_neighbor",
                    "rho_neighbor",
                    "the name of the lattice density material property");
  return params;
}

ADMatInterfaceDiffusion::ADMatInterfaceDiffusion(const InputParameters & parameters)
  : ADInterfaceKernel(parameters),
  _D(getADMaterialProperty<Real>("D")),
  _D_neighbor(getNeighborADMaterialProperty<Real>("D_neighbor")),
  _rho(getADMaterialProperty<Real>("rho")),
  _rho_neighbor(getNeighborADMaterialProperty<Real>("rho_neighbor"))
{
}

ADReal
ADMatInterfaceDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  // equal gradients means difference is zero
  ADReal res =
      ( _D[_qp] * _grad_u[_qp] * _normals[_qp])
       - ((_rho_neighbor[_qp]/_rho[_qp]) * _D_neighbor[_qp] * _grad_neighbor_value[_qp] * _normals[_qp]);

  switch (type)
  {
    case Moose::Element:
      return res * _test[_i][_qp]; // * -1?

    case Moose::Neighbor:
      return res * (_rho[_qp] / _rho_neighbor[_qp]) * _test_neighbor[_i][_qp];
  }

  mooseError("Internal error.");
}