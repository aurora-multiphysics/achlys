//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
 
#pragma once
 
#include "ADIntegratedBC.h"
 
class ADSurfaceRecombination;
 
// template <>
// InputParameters validParams<CoolantRecombination>();
 
class ADSurfaceRecombination : public ADIntegratedBC
{
public:
  static InputParameters validParams();
 
  ADSurfaceRecombination(const InputParameters & parameters);
 
protected:
  virtual ADReal computeQpResidual() override;

  Real _T;
  Real _K0;
  Real _E;
  const Real _scale_factor;

};