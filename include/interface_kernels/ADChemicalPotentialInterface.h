#pragma once

#include "ADInterfaceKernel.h"

/**
 * DG kernel for enforcing continuity of chemical potential between two variables on adjacent blocks
 */
class ADChemicalPotentialInterface : public ADInterfaceKernel
{
public:
  static InputParameters validParams();

  ADChemicalPotentialInterface(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual(Moose::DGResidualType type) override;

  const ADMaterialProperty<Real> & _s;
  const ADMaterialProperty<Real> & _s2;
  const ADMaterialProperty<Real> & _rho;
  const ADMaterialProperty<Real> & _rho2;
};