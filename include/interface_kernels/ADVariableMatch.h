#pragma once

#include "ADInterfaceKernel.h"

/**
 * DG kernel for enforcing continuity of chemical potential between two variables on adjacent blocks
 */
class ADVariableMatch : public ADInterfaceKernel
{
public:
  static InputParameters validParams();

  ADVariableMatch(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual(Moose::DGResidualType type) override;

    const ADMaterialProperty<Real> & _rho;
    const ADMaterialProperty<Real> & _rho_neighbor;

};