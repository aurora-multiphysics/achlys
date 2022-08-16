#pragma once

#include "GeneralPostprocessor.h"
#include "Coupleable.h"

class SideMaxValue;

class SideMaxValue : public GeneralPostprocessor, public Coupleable
{
public:
  static InputParameters validParams();

  SideMaxValue(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;

  virtual PostprocessorValue getValue() override;

protected:
  Real getValueAtPoint(const Point & p);

  ///@{ line to sample along
  const Point _start_point;
  const Point _end_point;
  const Real _length;
  ///@}

  /// search depth
  const unsigned int _depth;

  /// coupled variable
  MooseVariable & _coupled_var;

  /// detected interface location
  Real _extreme_value;

  /// The Mesh we're using
  MooseMesh & _mesh;

  /// So we don't have to create and destroy the dummy vector
  std::vector<Point> _point_vec;

  /// helper object to locate elements containing points
  std::unique_ptr<PointLocatorBase> _pl;
};
