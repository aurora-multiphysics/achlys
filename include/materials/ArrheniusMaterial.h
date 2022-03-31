// This file is part of the MOOSE framework
// https://www.mooseframework.org
#pragma once

#include "ADMaterial.h"
// #include "LinearInterpolation.h"

class ArrheniusMaterial;

template <>
InputParameters validParams<ArrheniusMaterial>();

class ArrheniusMaterial : public ADMaterial
{
public:
  ArrheniusMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

private:
    std::string _name;
    Real _v0;
    Real _E;
    Real _k;
    const ADVariableValue & _T;
    ADMaterialProperty<Real> & _property;
};