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
  params.addRequiredParam<std::vector<Real>>("ni", "possible trapping sites");
  params.addRequiredParam<std::vector<std::string>>("ni_names", "material property names");
  params.addRequiredParam<std::vector<std::string>>("vi_names", "material property names");
  params.addRequiredParam<Real>("k_boltz", "Boltzman constant");
  params.addRequiredParam<Real>("D0", "The diffusion pre-exponential factor");
  params.addRequiredParam<Real>("E_diff", "diffusion energy in eV");
  params.addRequiredParam<Real>("lambda", "Lattice constant in m-1");
  params.addRequiredParam<Real>("n_sol", "density of interstitial sites");
  params.addCoupledVar("Temperature","simulation temperature");

  return params;
}

VectorTrappingMaterial::VectorTrappingMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _v0(getParam<std::vector<Real>>("v0")),
    _Ei(getParam<std::vector<Real>>("Ei")),
    _ni(getParam<std::vector<Real>>("ni")),
    _ni_names(getParam<std::vector<std::string>>("ni_names")),
    _vi_names(getParam<std::vector<std::string>>("vi_names")),
    _k_boltz(getParam<Real>("k_boltz")),
    _D0(getParam<Real>("D0")),
    _E_diff(getParam<Real>("E_diff")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _T(adCoupledValue("Temperature")),
    _D(declareADProperty<Real>("D")),
    _Vm(declareADProperty<Real>("Vm"))
    // _n(declareADProperty<RealEigenVector>("ni")),
    // _Vi(declareADProperty<RealEigenVector>("Vi"))
   
{
  // test vectors are all same length
  bool equal_arrays = _v0.size() == _ni.size() && _Ei.size() == _ni.size();
  if (! equal_arrays)
  {
    mooseError("The trap parameter vectors v0, ni, and Ei must be the same length");
  }

  _num_traps = _ni.size();
  _n.resize(_num_traps);
  _Vi.resize(_num_traps);
  for (unsigned int i = 0; i < _num_traps; i++)
  {
    _n[i] = &declareADProperty<Real>(_ni_names[i]);
    _Vi[i] = &declareADProperty<Real>(_vi_names[i]);
  }

}

ADReal VectorTrappingMaterial::Arhhenious(Real v0, Real E)
{
  return v0 * std::exp((-1.0 * E) / (_k_boltz * _T[_qp]));
}

void
VectorTrappingMaterial::computeQpProperties()
{

  _D[_qp] = _D0 * std::exp((-1.0 * _E_diff) / (_k_boltz * _T[_qp]));
  _Vm[_qp] = _D[_qp] / ( std::pow(_lambda, 2) * _n_sol);
  for (unsigned int i = 0; i < _num_traps; i++)
  {
    (*_n[i])[_qp] = _ni[i];
    (*_Vi[i])[_qp] = Arhhenious(_v0[i],_Ei[i]);
  }

  // _n[_qp] = _ni;
  // _n[_qp] = _v0;
  // if (_Vi[_qp].size() != _v0.size()) 
  // {
  //   _Vi[_qp].resize(_v0.size());
  // }
  // for(int i=0; i < _v0.size(); i++)
  // {
  //   _Vi[_qp](i) = Arhhenious(_v0[i], _Ei[i]);
  // }

  // // do nothing if trap parameters are empty
  // if (!_ni.empty())
  // {
  //   // initialise material arrays
  //   if ((_n[_qp]).empty()) 
  //   {
  //     // copy vector of trap densities into the ni material property
  //     std::copy(_ni.begin(), _ni.end(),
  //                   std::back_inserter(_n[_qp]));

  //     // apply arhennious function to the trapping rate material property using each value in the pair of input vectors.
  //     std::transform(
  //       _v0.begin(), _v0.end(), _Ei.begin(), 
  //       std::back_inserter(_Vi[_qp]), 
  //       [this] (Real i, Real j) -> ADReal {return this->Arhhenious(i, j);}
  //       );
  //   }
  //   else
  //   {
  //     // number density is assumed constant in this model so no update required after initialisation
  //     std::transform(
  //       _v0.begin(), _v0.end(), _Ei.begin(), 
  //       (_Vi[_qp]).begin(), 
  //       [this] (Real i, Real j) -> ADReal {return this->Arhhenious(i, j);}
  //       );
  //   }
  // }
}