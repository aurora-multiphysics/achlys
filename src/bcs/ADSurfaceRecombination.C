#include "ADSurfaceRecombination.h"

registerMooseObject("achlysApp", ADSurfaceRecombination);

// defineLegacyParams(CoupledVarNeumannBC);

InputParameters
ADSurfaceRecombination::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addParam<Real>(
      "scale_factor", 1.0, "Coefficent ($\\sigma$) multiplier for the coupled force term.");
  params.addClassDescription("Imposes the integrated boundary condition "
                             "$\\frac{\\partial u}{\\partial n}= kv^2$, "
                             "where $v$ is a variable and the constant, k, "
                             " is calculated from an Arhhenious expression based on"
                             " provided (constant) values of E and T");
  params.addRequiredParam<Real>("Temperature","simulation temperature");
  params.addRequiredParam<Real>("k0","pre-exponent factor");
  params.addRequiredParam<Real>("E","Energy Barrier in Arhhenious Expression");
  return params;
}

ADSurfaceRecombination::ADSurfaceRecombination(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _T(getParam<Real>("Temperature")),
    _K0(getParam<Real>("k0")),
    _E(getParam<Real>("E")),
    _scale_factor(getParam<Real>("scale_factor"))
{
}
ADReal
ADSurfaceRecombination::computeQpResidual()
{
  if (_u[_qp] > 0.0)
  {
    Real _K = _K0 * std::exp((-1.0 * _E) / (8.617333E-5 * _T) );
    ADReal scaled_c_squared = _scale_factor * _u[_qp] * _u[_qp] ;
    return  _K * _test[_i][_qp] * scaled_c_squared;
  }
  else
  {
    return _test[_i][_qp] * 0; 
  }
}
