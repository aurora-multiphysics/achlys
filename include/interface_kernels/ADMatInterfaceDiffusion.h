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
    // const MooseArray<Point> & _qp_neighbor;
    // const QBase* _qrule_neighbour;
    // const VariableValue & _T;
    // Real _D0_neighbour;
    // Real _Ed_neighbour;

};