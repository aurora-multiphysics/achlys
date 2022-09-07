#pragma once
#include "ADInterfaceKernel.h"

/*
 * Enforce gradient continuity between two different variables across a
 * subdomain boundary.
 */
class ADMatInterfaceDiffusion : public ADInterfaceKernel
{
public:
  static InputParameters validParams();

  ADMatInterfaceDiffusion(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual(Moose::DGResidualType type) override;
//   virtual Real computeQpJacobian(Moose::DGJacobianType type) override;

private:
    const ADMaterialProperty<Real> & _D;
    const ADMaterialProperty<Real> & _D_neighbor;
    const ADMaterialProperty<Real> & _rho;
    const ADMaterialProperty<Real> & _rho_neighbor;
    // const MooseArray<Point> & _qp_neighbor;
    // const QBase* _qrule_neighbor;
    // const VariableValue & _T;
    // Real _D0_neighbor;
    // Real _Ed_neighbour;

};