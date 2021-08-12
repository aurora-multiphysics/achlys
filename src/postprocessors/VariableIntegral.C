//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//  This post processor returns the mass value of an element.  It is used
//  to check that mass is conserved (per the evolving density calculation)
//  when volume changes occur.
//
#include "VariableIntegral.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("achlysApp", VariableIntegral);


InputParameters
VariableIntegral::validParams()
{
  InputParameters params = ElementIntegralVariablePostprocessor::validParams();
  params.addCoupledVar("variable","Nonlinear variable to integrate over simulation domain");
  return params;
}

// template <bool is_ad>
VariableIntegral::VariableIntegral(const InputParameters & parameters)
  : ElementIntegralVariablePostprocessor(parameters),
  _variable(adCoupledValue("variable"))
{
}

// template <bool is_ad>
Real
VariableIntegral::computeQpIntegral()
{
  return MetaPhysicL::raw_value(_variable[_qp]);
}

// template class MassTempl<false>;s
// template class MassTempl<true>;
