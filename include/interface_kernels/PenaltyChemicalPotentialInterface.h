#pragma once

#include "ADInterfaceKernel.h"

/**
 * DG kernel for enforcing continuity of chemical potential between two variables on adjacent blocks
 */
class PenaltyChemicalPotentialInterface : public ADInterfaceKernel
{
public:
  static InputParameters validParams();

  PenaltyChemicalPotentialInterface(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual(Moose::DGResidualType type) override;

  const ADMaterialProperty<Real> & _s;
  const ADMaterialProperty<Real> & _s_neighbor;
  const ADMaterialProperty<Real> & _rho;
  const ADMaterialProperty<Real> & _rho_neighbor;
  Real _p;
};