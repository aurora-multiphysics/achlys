#pragma once

#include "GeneralPostprocessor.h"
#include "Coupleable.h"

class ValueAtPoint;


/**
 * Find a specific target value along a sampling line. The variable values along
 * the line should change monotonically. The target value is searched using a
 * bisection algorithm.
 * The Postprocessor reports the distance from the start_point along the line
 * between start_point and end_point.
 */
class ValueAtPoint : public GeneralPostprocessor, public Coupleable
{
public:
  static InputParameters validParams();

  ValueAtPoint(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;

  virtual PostprocessorValue getValue() override;

protected:
  Real getValueAtPoint(const Point & p);

  ///@{ line to sample along
  const Point _position;
  ///@}

  /// coupled variable
  MooseVariable & _coupled_var;

  /// detected interface location
  Real _value;

  /// The Mesh we're using
  MooseMesh & _mesh;

  /// So we don't have to create and destroy the dummy vector
  std::vector<Point> _point_vec;

  /// helper object to locate elements containing points
  std::unique_ptr<PointLocatorBase> _pl;
};
