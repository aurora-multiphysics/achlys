//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "SideIntegralVariablePostprocessor.h"


class SideDesorptionFlux;

InputParameters validParams();

/**
 * This postprocessor computes a side integral of the mass flux.
 */
class SideDesorptionFlux : public SideIntegralVariablePostprocessor
{
public:
  static InputParameters validParams();

  SideDesorptionFlux(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;
    const VariableValue & _T;
    Real _K0;
    Real _E;
    const Real _scale_factor;
    const ADVariableValue & _variable;

};
