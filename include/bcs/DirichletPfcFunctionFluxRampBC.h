//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// #include "DirichletBCBase.h"
#include "ADIntegratedBC.h"

/**
 * Boundary condition of a Dirichlet type
 *
 * Sets the values of a nodal variable at nodes
 */
class DirichletPfcFunctionFluxRampBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  DirichletPfcFunctionFluxRampBC(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

    const VariableValue & _T;
    Real _threshold;
    Real _K0;
    Real _E;
    Real _scale_factor;
    const Function & _flux_function;
};
