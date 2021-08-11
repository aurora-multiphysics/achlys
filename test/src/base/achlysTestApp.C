//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "achlysTestApp.h"
#include "achlysApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
achlysTestApp::validParams()
{
  InputParameters params = achlysApp::validParams();
  return params;
}

achlysTestApp::achlysTestApp(InputParameters parameters) : MooseApp(parameters)
{
  achlysTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

achlysTestApp::~achlysTestApp() {}

void
achlysTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  achlysApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"achlysTestApp"});
    Registry::registerActionsTo(af, {"achlysTestApp"});
  }
}

void
achlysTestApp::registerApps()
{
  registerApp(achlysApp);
  registerApp(achlysTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
achlysTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  achlysTestApp::registerAll(f, af, s);
}
extern "C" void
achlysTestApp__registerApps()
{
  achlysTestApp::registerApps();
}
