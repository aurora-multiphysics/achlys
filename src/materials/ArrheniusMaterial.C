#include "ArrheniusMaterial.h"

registerMooseObject("achlysApp", ArrheniusMaterial);

template <>
InputParameters
validParams<ArrheniusMaterial>()
{
  InputParameters params = validParams<ADMaterial>();
  params.addRequiredParam<std::string>("name", "The name for this material property");
  params.addRequiredParam<Real>("v0", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<Real>("E", "Trap detrapping energy in eV");
  params.addRequiredParam<Real>("k", "Boltzman constant or universal gas constant");
  params.addCoupledVar("Temperature","simulation temperature");

  return params;
}

ArrheniusMaterial::ArrheniusMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _name(getParam<std::string>("name")),
    _v0(getParam<Real>("v0")),
    _E(getParam<Real>("E")),
    _k(getParam<Real>("k")),
    _T(adCoupledValue("Temperature")),
    _property(declareADProperty<Real>(_name))
    // _property(declareADProperty<Real>(_name));
{
  // _property = declareADProperty<Real>(_name);
}

void
ArrheniusMaterial::computeQpProperties()
{
  _property[_qp] = _v0 * std::exp((-1.0 * _E) / (_k * _T[_qp]));
}