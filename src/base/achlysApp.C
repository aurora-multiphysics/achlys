#include "achlysApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
achlysApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output, i.e., output properties on INITIAL as well as TIMESTEP_END
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

achlysApp::achlysApp(InputParameters parameters) : MooseApp(parameters)
{
  achlysApp::registerAll(_factory, _action_factory, _syntax);
}

achlysApp::~achlysApp() {}

void
achlysApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"achlysApp"});
  Registry::registerActionsTo(af, {"achlysApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
achlysApp::registerApps()
{
  registerApp(achlysApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
achlysApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  achlysApp::registerAll(f, af, s);
}
extern "C" void
achlysApp__registerApps()
{
  achlysApp::registerApps();
}
