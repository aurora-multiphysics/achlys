#include "ADSurfaceRecombinationCoupledT.h"

registerMooseObject("achlysApp", ADSurfaceRecombinationCoupledT);

// defineLegacyParams(CoupledVarNeumannBC);

InputParameters
ADSurfaceRecombinationCoupledT::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addParam<Real>(
      "scale_factor", 1.0, "Coefficent ($\\sigma$) multiplier for the coupled force term.");
  params.addClassDescription("Imposes the integrated boundary condition "
                             "$\\frac{\\partial u}{\\partial n}= kv^2$, "
                             "where $v$ is a variable and the constant, k, "
                             " is calculated from an Arhhenious expression based on"
                             " an energy barrier, E, and a coupled Temperature.");
  params.addRequiredCoupledVar("Temperature", "Temperature");
  params.addRequiredParam<Real>("k0","pre-exponent factor");
  params.addRequiredParam<Real>("E","Energy Barrier in Arhhenious Expression");
  return params;
}

ADSurfaceRecombinationCoupledT::ADSurfaceRecombinationCoupledT(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _T(coupledValue("Temperature")),
    _K0(getParam<Real>("k0")),
    _E(getParam<Real>("E")),
    _scale_factor(getParam<Real>("scale_factor"))
{
}
ADReal
ADSurfaceRecombinationCoupledT::computeQpResidual()
{
  Real _K = _K0 * std::exp((-1.0 * _E) / (8.617333E-5 * MetaPhysicL::raw_value(_T[_qp])) );
  ADReal scaled_c_squared = _scale_factor * _u[_qp] *_u[_qp];
    return  _K * _test[_i][_qp] * scaled_c_squared;
}