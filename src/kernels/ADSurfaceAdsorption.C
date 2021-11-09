#include "ADSurfaceAdsorption.h"

registerMooseObject("achlysApp", ADSurfaceAdsorption);

InputParameters
ADSurfaceAdsorption::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Compute the diffusion term for Darcy pressure ($p$) equation: "
                             "$-\\nabla \\cdot \\frac{\\mathbf{K}}{\\mu} \\nabla p = 0$");
  params.addParam<MaterialPropertyName>("Gamma", "Gamma", "Incident particle flux");
  params.addParam<MaterialPropertyName>("Vd", "Vd", "Desorption rate");
  params.addParam<MaterialPropertyName>("Vsb", "Vd", "Rate of transport from surface to bulk");
  params.addParam<MaterialPropertyName>("Sigma", "Sigma", "Cross section for recombination of incident particle with trapped one");
  params.addParam<MaterialPropertyName>("n_surf", "n_surf", "Desorption rate");
  params.addParam<MaterialPropertyName>("S", "S", "Sticking probability");
  return params;
}

ADSurfaceAdsorption::ADSurfaceAdsorption(const InputParameters & parameters)
  : ADKernel(parameters),
    _gamma(getADMaterialProperty<Real>("Gamma")),
    _vd(getADMaterialProperty<Real>("Vd")),
    _vsb(getADMaterialProperty<Real>("Vsb")),
    _sigma(getADMaterialProperty<Real>("Sigma")),
    _n_surf(getADMaterialProperty<Real>("n_surf")),
    _s(getADMaterialProperty<Real>("S"))
{
}

ADReal
ADSurfaceAdsorption::computeQpResidual()
{
  ADReal in = 2.0 * _gamma[_qp] * _s[_qp] * std::pow(1.0-_u[_qp], 2) - std::abs(_gamma[_qp] * _u[_qp] * _sigma[_qp]);
  ADReal out = (2.0 * _vd[_qp] * _u[_qp] * _u[_qp]);
    // + (_vsb[_qp] * _u[_qp]) 

  if (_u[_qp] >= 1.0)
  {
    in = 0.0;
  }

  if (_u[_qp] < 0.0)
  {
    out = 0.0;
  }

  return -1.0 * _test[_i][_qp] * (in - out);
  
}

