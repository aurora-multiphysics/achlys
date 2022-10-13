#include "achlysApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"
#include "ActionFactory.h"

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

static void
associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
{
//   // Create the syntax
//   registerSyntax("SimpleTrapAction", "Modules/AchlysTrap");
  // registerSyntax("FosterMcNabbTrapAction", "Modules/Achlys/FosterMcNabb");
   registerSyntax("FosterMcNabbTrapAction", "Modules/Achlys/FosterMcNabb/*");
   registerSyntax("DiffusiveMaterialAction", "Modules/Achlys/DiffusiveMaterial/*");

//   // add variables action
//   registerTask("add_fm_variables", /*is_required=*/false);
//   addTaskDependency("add_fm_variables", "add_variable");

// //   // add ICs action
// //   registerTask("add_trap_ics", /*is_required=*/false);
// //   addTaskDependency("add_trap_ics", "add_ic");

//   // add Materials action
//   registerTask("add_fm_materials", /*is_required=*/false);
//   addTaskDependency("add_fm_materials", "add_material");

//   // add Kernels action
//   registerTask("add_fm_kernels", /*is_required=*/false);
//   addTaskDependency("add_fm_kernels", "add_kernel");

//   // add interface  Kernels action
//   registerTask("add_fm_interface_kernels", /*is_required=*/false);
//   addTaskDependency("add_fm_interface_kernels", "add_interface_kernel");

//   // add aux variable action
//   registerTask("add_fm_aux_variables", /*is_required=*/false);
//   addTaskDependency("add_fm_aux_variables", "add_aux_variable");

//   // add aux kernel action
//   registerTask("add_fm_aux_kernels", /*is_required=*/false);
//   addTaskDependency("add_fm_aux_kernels", "add_aux_kernel");

//   // add BCs actions
//   registerTask("add_trap_bcs", /*is_required=*/false);
//   addTaskDependency("add_trap_bcs", "add_bc");
}

void
achlysApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"achlysApp"});
  Registry::registerActionsTo(af, {"achlysApp"});

  /* register custom execute flags, action syntax, etc. here */
  associateSyntaxInner(syntax, af);
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
