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
// using integratedBC for now as has variable and material proprty coupling built in
//  need new couplable Dirichlet base class to proceed otherwise
 
// #include "ADDirichletBCBase.h"
// #include "MaterialPropertyInterface.h"

/**
 * Boundary condition of a Dirichlet type
 *
 * Sets the values of a nodal variable at nodes
 */
// class DirichletEquivalentImplantBC : public ADDirichletBCBase, public MaterialPropertyInterface
class DirichletEquivalentImplantBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  DirichletEquivalentImplantBC(const InputParameters & parameters);

protected:
//   virtual ADReal computeQpValue() override;
    virtual ADReal computeQpResidual() override;

    Real _flux;
    Real _R;
    const ADMaterialProperty<Real> & _D;
    const ADMaterialProperty<Real> & _rho;
};
