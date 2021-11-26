#include "ADVectorTrappingEquilibriumEquation.h"

registerMooseObject("achlysApp", ADVectorTrappingEquilibriumEquation);

InputParameters
ADVectorTrappingEquilibriumEquation::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Compute the local movement of particles between the mobile phase"
                             "and the trapped phase for a specific trap type");
  params.addRequiredCoupledVar("v", "Concentration of mobile particles");
  params.addParam<MaterialPropertyName>("n_traps", "n", "material property for trapping density");
  params.addParam<MaterialPropertyName>("vi", "Vi", "the rate constant for this trap");
  params.addParam<MaterialPropertyName>("Vm", "Vm", "the rate constant the trapping process");
  params.addRequiredParam<Real>("index", "the trap index");
  return params;
}

ADVectorTrappingEquilibriumEquation::ADVectorTrappingEquilibriumEquation(const InputParameters & parameters)
  : ADKernel(parameters),
    _v(adCoupledValue("v")),
    _v_m(getADMaterialProperty<Real>("Vm")),
    _v_i(getADMaterialProperty<std::vector<Real>>("vi")),
    _n_sites(getADMaterialProperty<std::vector<Real>>("n_traps")),
    _idx(getParam<Real>("index"))
{
}

ADReal
ADVectorTrappingEquilibriumEquation::computeQpResidual()
{
  ADReal detrapping = _v_i[_qp][_idx] * _u[_qp];
  ADReal trapping = _v_m[_qp] * _v[_qp] * (_n_sites[_qp][_idx] - _u[_qp]);
  
  return -1.0 * _test[_i][_qp] * (trapping - detrapping);
}
 