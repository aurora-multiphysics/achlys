
#include "ADCoupledIsotopeDiffusion.h"

registerMooseObject("achlysApp", ADCoupledIsotopeDiffusion);

InputParameters
ADCoupledIsotopeDiffusion::validParams()
{
  auto params = ADKernelGrad::validParams();
  params.addClassDescription("Same as `Diffusion` in terms of physics/residual, but the Jacobian "
                             "is computed using forward automatic differentiation");
  params.addRequiredCoupledVar("v", "Concentration of mobile coupled isotope");
  params.addParam<MaterialPropertyName>("D1", "D1", "Diffusion coefficient for primary isotope");
  params.addParam<MaterialPropertyName>("D2", "D2", "Diffusion coefficient for coupled isotope");
  return params;
}

ADCoupledIsotopeDiffusion::ADCoupledIsotopeDiffusion(const InputParameters & parameters)
 : ADKernelGrad(parameters),
 _v(adCoupledValue("v")),
 _grad_v(adCoupledGradient("v")),
 _D1(getADMaterialProperty<Real>("D1")),
 _D2(getADMaterialProperty<Real>("D2"))
 {}

ADRealVectorValue
ADCoupledIsotopeDiffusion::precomputeQpResidual()
{
  ADReal f = (_v[_qp] + _u[_qp] > 0.0) ? (_u[_qp] / (_v[_qp] + _u[_qp]) ) : 0.5;
  ADReal D = (_D1[_qp] - _D2[_qp]) * f;
  return D * _grad_v[_qp];
}
