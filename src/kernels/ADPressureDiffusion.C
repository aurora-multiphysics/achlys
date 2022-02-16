#include "ADPressureDiffusion.h"

registerMooseObject("achlysApp", ADPressureDiffusion);

InputParameters
ADPressureDiffusion::validParams()
{
  auto params = ADMatDiffusionBase<Real>::validParams();
  params.addParam<Real>("R", 8.314, "Universal gas constant");
  params.addParam<Real>("Vh", 1.6e-6, "Partial molar volume");
  params.addRequiredCoupledVar("v", "Hydrostatic stress variable");
  params.addRequiredCoupledVar("T", "Temperature");
  params.addClassDescription(
      "Diffusion equation kernel that takes an isotropic diffusivity from a material property");
  return params;
}

ADPressureDiffusion::ADPressureDiffusion(const InputParameters & parameters)
  : ADMatDiffusionBase<Real>(parameters),
  _T(adCoupledValue("T")),
  _R(getParam<Real>("R")),
  _Vh(getParam<Real>("Vh"))
{
}

ADRealVectorValue
ADPressureDiffusion::precomputeQpResidual()
{
  return (_diffusivity[_qp] * _u[_qp] * _Vh / (_R * _T[_qp])) * _grad_v[_qp];
}