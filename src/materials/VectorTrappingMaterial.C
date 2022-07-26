#include "VectorTrappingMaterial.h"
#include <cstdio>
#include <algorithm>
#include <iterator>

registerMooseObject("achlysApp", VectorTrappingMaterial);


InputParameters VectorTrappingMaterial::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addRequiredParam<std::vector<Real>>("v0", "pre-exponential detrapping factor in Arrhenious eq.");
  params.addRequiredParam<std::vector<Real>>("Ei", "Trap detrapping energy in eV");
  params.addRequiredParam<Real>("k_boltz", "Boltzman constant");
  params.addRequiredParam<Real>("D0", "The diffusion pre-exponential factor");
  params.addRequiredParam<Real>("E_diff", "diffusion energy in eV");
  params.addRequiredParam<Real>("lambda", "Lattice constant in m-1");
  params.addRequiredParam<Real>("n_sol", "density of interstitial sites");
  params.addRequiredParam<std::vector<Real>>("ni", "possible trapping sites");
  params.addCoupledVar("Temperature","simulation temperature");

  return params;
}

VectorTrappingMaterial::VectorTrappingMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _v0(getParam<std::vector<Real>>("v0")),
    _Ei(getParam<std::vector<Real>>("Ei")),
    _k_boltz(getParam<Real>("k_boltz")),
    _D0(getParam<Real>("D0")),
    _E_diff(getParam<Real>("E_diff")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _ni(getParam<std::vector<Real>>("ni")),
    _T(adCoupledValue("Temperature")),
    _D(declareADProperty<Real>("D")),
    _Vm(declareADProperty<Real>("Vm")),
    _Vi(declareADProperty<std::vector<Real>>("Vi")),
    _n(declareADProperty<std::vector<Real>>("ni"))
{
}

ADReal VectorTrappingMaterial::Arhhenious(Real v0, Real E)
{
  return v0 * std::exp((-1.0 * E) / (_k_boltz * _T[_qp]));
}

void
VectorTrappingMaterial::computeQpProperties()
{
  // test vectors are all same length
  bool equal_arrays = _v0.size() == _ni.size() && _Ei.size() == _ni.size();
  if (! equal_arrays)
  {
    mooseError("The trap parameter vectors v0, ni, and Ei must be the same length");
  }

  _D[_qp] = _D0 * std::exp((-1.0 * _E_diff) / (_k_boltz * _T[_qp]));
  _Vm[_qp] = _D[_qp] / ( std::pow(_lambda, 2) * _n_sol);

  // copy vector of trap densities into the ni material property
  std::copy(_ni.begin(), _ni.end(),
                std::back_inserter(_n[_qp]));

  // apply arhennious function to the trapping rate material property using each value in the pair of input vectors.
  std::transform(
    _v0.begin(), _v0.end(), _Ei.begin(), 
    std::back_inserter(_Vi[_qp]), 
    [this] (Real i, Real j) -> ADReal {return this->Arhhenious(i, j);}
    );
}