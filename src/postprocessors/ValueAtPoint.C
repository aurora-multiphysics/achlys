//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ValueAtPoint.h"

// MOOSE includes
#include "MooseMesh.h"
#include "MooseUtils.h"
#include "MooseVariable.h"

registerMooseObject("achlysApp", ValueAtPoint);

// defineLegacyParams(FindValueOnLine);

InputParameters
ValueAtPoint::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addClassDescription("Find a specific target value along a sampling line. The variable "
                             "values along the line should change monotonically. The target value "
                             "is searched using a bisection algorithm.");
  params.addParam<Point>("position", "Start point of the sampling line.");
  params.addCoupledVar("v", "Variable to inspect");
  return params;
}

ValueAtPoint::ValueAtPoint(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    Coupleable(this, false),
    _position(getParam<Point>("position")),
    _coupled_var(*getVar("v", 0)),
    _value(0.0),
    _mesh(_subproblem.mesh()),
    _point_vec(1)
{
}

void
ValueAtPoint::initialize()
{
  // We do this here just in case it's been destroyed and recreated becaue of mesh adaptivity.
  _pl = _mesh.getPointLocator();
  _pl->enable_out_of_mesh_mode();
}

void
ValueAtPoint::execute()
{
    _value = getValueAtPoint(_position);
}

Real
ValueAtPoint::getValueAtPoint(const Point & p)
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
ValueAtPoint::getValue()
{
  return _value;
}
