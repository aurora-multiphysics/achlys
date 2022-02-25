#pragma once

#include "ADInterfaceKernel.h"

/**
 * DG kernel for enforcing continuity of chemical potential between two variables on adjacent blocks
 */
class ADChemicalPotentialInterface2 : public ADInterfaceKernel
{
public:
  static InputParameters validParams();

  ADChemicalPotentialInterface2(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual(Moose::DGResidualType type) override;

  const ADMaterialProperty<Real> & _s;
  const ADMaterialProperty<Real> & _s_neighbour;
  Real _p;
};