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
  const ADMaterialProperty<Real> & _n_surf;
  const ADMaterialProperty<Real> & _lambda;
};