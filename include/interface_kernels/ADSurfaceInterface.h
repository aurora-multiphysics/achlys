// This file is part of the MOOSE framework
// https://www.mooseframework.org
//
// All rights reserved, see COPYRIGHT for full restrictions
// https://github.com/idaholab/moose/blob/master/COPYRIGHT
//
// Licensed under LGPL 2.1, please see LICENSE for details
// https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADInterfaceKernel.h"

/**
 * DG kernel for interfacing diffusion between two variables on adjacent blocks
 */
class ADSurfaceInterface : public ADInterfaceKernel
{
public:
  static InputParameters validParams();

  ADSurfaceInterface(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual(Moose::DGResidualType type) override;
//   virtual Real computeQpJacobian(Moose::DGJacobianType type) override;

  const ADMaterialProperty<Real> & _vd;
  const ADMaterialProperty<Real> & _vsb;
  const ADMaterialProperty<Real> & _vbs;
  const ADMaterialProperty<Real> & _sigma;
  const ADMaterialProperty<Real> & _S;
  const ADMaterialProperty<Real> & _Gamma;

  const ADMaterialProperty<Real> & _D_neighbor;
  Real _n_surf;
  Real _lambda;
};