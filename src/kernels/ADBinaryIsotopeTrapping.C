#include "ADBinaryIsotopeTrapping.h"

registerMooseObject("achlysApp", ADBinaryIsotopeTrapping);

InputParameters
ADBinaryIsotopeTrapping::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Compute the diffusion term for Darcy pressure ($p$) equation: "
                             "$-\\nabla \\cdot \\frac{\\mathbf{K}}{\\mu} \\nabla p = 0$");
  params.addRequiredCoupledVar("cm", "Concentration of mobile particles for this isotope");
  params.addRequiredCoupledVar("c2", "Concentration of second isotope in this trap");
  params.addParam<MaterialPropertyName>("n_traps", "n0", "material property for trapping density");
  params.addParam<MaterialPropertyName>("vi", "v0", "the rate constant for this trap");
  params.addParam<MaterialPropertyName>("vm", "vm1", "the rate constant for the trapping process");
  return params;
}

ADBinaryIsotopeTrapping::ADBinaryIsotopeTrapping(const InputParameters & parameters)
  : ADKernel(parameters),
    //_v(coupledValues("v")),
    _v(adCoupledValue("cm")),
    _c2(adCoupledValue("c2")),
    _v_m(getADMaterialProperty<Real>("vm")),
    _v_i(getADMaterialProperty<Real>("vi")),
    _n_sites(getADMaterialProperty<Real>("n_traps"))
{
}

ADReal
ADBinaryIsotopeTrapping::computeQpResidual()
{
  
  ADReal detrapping = _v_i[_qp] * _u[_qp];
  ADReal trapping = _v_m[_qp] * _v[_qp] * (_n_sites[_qp] - _u[_qp] - _c2[_qp]);

    return -1.0 * _test[_i][_qp] * (trapping - detrapping);
}

