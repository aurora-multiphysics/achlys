#include "ADTrappingCoupledDensity.h"

registerMooseObject("achlysApp", ADTrappingCoupledDensity);

InputParameters
ADTrappingCoupledDensity::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Compute the local movement of particles between the mobile phase"
                             "and the trapped phase for a specific trap type");
  params.addRequiredCoupledVar("v", "Concentration of mobile particles");
  params.addParam<MaterialPropertyName>("n_traps", "n0", "material property for trapping density");
  params.addParam<MaterialPropertyName>("vi", "v0", "the rate constant for this trap");
  params.addRequiredCoupledVar("n", "Concentration of trapping sites");
  return params;
}

ADTrappingCoupledDensity::ADTrappingCoupledDensity(const InputParameters & parameters)
  : ADKernel(parameters),
    _v(adCoupledValue("v")),
    _v_m(getADMaterialProperty<Real>("Vm")),
    _v_i(getADMaterialProperty<Real>("vi")),
    _n(adCoupledValue("n"))
{
}

ADReal
ADTrappingCoupledDensity::computeQpResidual()
{
  ADReal detrapping = _v_i[_qp] * _u[_qp];
  ADReal trapping = _v_m[_qp] * _v[_qp] * (_n[_qp] - _u[_qp]);
  
  return -1.0 * _test[_i][_qp] * (trapping - detrapping);
}
 