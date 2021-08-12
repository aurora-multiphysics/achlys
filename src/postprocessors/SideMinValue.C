//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SideMinValue.h"

// MOOSE includes
#include "MooseMesh.h"
#include "MooseUtils.h"
#include "MooseVariable.h"

registerMooseObject("achlysApp", SideMinValue);

// defineLegacyParams(FindValueOnLine);

InputParameters
SideMinValue::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addClassDescription("Find a specific target value along a sampling line. The variable "
                             "values along the line should change monotonically. The target value "
                             "is searched using a bisection algorithm.");
  params.addParam<Point>("start_point", "Start point of the sampling line.");
  params.addParam<Point>("end_point", "End point of the sampling line.");
  params.addParam<unsigned int>("depth", 100, "Maximum number of bisections to perform.");
  params.addCoupledVar("v", "Variable to inspect");
  return params;
}

SideMinValue::SideMinValue(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    Coupleable(this, false),
    _start_point(getParam<Point>("start_point")),
    _end_point(getParam<Point>("end_point")),
    _length((_end_point - _start_point).norm()),
    _depth(getParam<unsigned int>("depth")),
    _coupled_var(*getVar("v", 0)),
    _extreme_value(0.0),
    _mesh(_subproblem.mesh()),
    _point_vec(1)
{
}

void
SideMinValue::initialize()
{
  // We do this here just in case it's been destroyed and recreated becaue of mesh adaptivity.
  _pl = _mesh.getPointLocator();
  _pl->enable_out_of_mesh_mode();
}

void
SideMinValue::execute()
{
  Real value = 0;
  for (unsigned int i = 0; i < _depth; ++i)
  {
    Point p = (i / (_depth - 1)) * (_end_point - _start_point) + _start_point;
    value = getValueAtPoint(p);
    if (i == 0){
        _extreme_value = value;
    } else if (value < _extreme_value){
        _extreme_value = value;
    }
  }
}

Real
SideMinValue::getValueAtPoint(const Point & p)
{
  const Elem * elem = (*_pl)(p);

  processor_id_type elem_proc_id = elem ? elem->processor_id() : DofObject::invalid_processor_id;
  _communicator.min(elem_proc_id);

  if (elem_proc_id == DofObject::invalid_processor_id)
  {
    // there is no element
    mooseError("No element found at the current search point. Please make sure the sampling line "
               "stays inside the mesh completely.");
  }

  Real value = 0;

  if (elem)
  {
    if (elem->processor_id() == processor_id())
    {
      // element is local
      _point_vec[0] = p;
      _subproblem.reinitElemPhys(elem, _point_vec, 0);
      value = _coupled_var.sln()[0];
    }
  }

  // broadcast value
  _communicator.broadcast(value, elem_proc_id);
  return value;
}

PostprocessorValue
SideMinValue::getValue()
{
  return _extreme_value;
}
