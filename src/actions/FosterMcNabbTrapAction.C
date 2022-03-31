// FosterMcNabbTrapAction
// "achlysApp"
#include "Conversion.h"
#include "FEProblem.h"
#include "Factory.h"
#include "MooseMesh.h"
#include "MooseObjectAction.h"
#include "TensorMechanicsAction.h"
#include "Material.h"

#include "BlockRestrictable.h"

// #include "HomogenizationConstraintIntegral.h" // just for the constants
#include "AddVariableAction.h"

#include "libmesh/string_to_enum.h"
#include <algorithm>

// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "meta_action");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "setup_mesh_complete");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "validate_coordinate_systems");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_variable");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_aux_variable");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_kernel");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_aux_kernel");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_material");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_master_action_material");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_scalar_kernel");
// registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_user_object");

/*
    1) create new trap_i variables for each element given in material property vectors (for each material subdomain)
        i) consider variable order and family

    2) declare new material properties 

    3) create 3 kernels for the Foster-McNabb trapping model, including coupling with the mobile term

    4) instantiate interface kernels if required

    5) create summary aux-variables which summarise global mobile / trapped HI in SI units
        i) option to disable if requested

    Questions:
    i) create mobile variables here too? -- how will the user-object ecosystem look?
    ii) global variable options? (seen some convergence issues just being block-restricted)
    iii) can this generalise to multi-isotope models?
    iv) switch between molar and atomic units? 

*/

InputParameters
TensorMechanicsAction::validParams()
{
//   InputParameters params = TensorMechanicsActionBase::validParams();
    InputParameters params = Action::validParams();
    params.addClassDescription("Set up kernels and materials for a Foster-McNabb trapping model");
    params.addParam<std::vector<MaterialPropertyName>>("material_definitions", "material classes providing input data");
    // interface type -- global variables or block restricted?
    // handle variable trap densities 
    // varying requirements for input variables e.g. solubility for CP interface but not otherwise
    return params;
}

FosterMcNabbTrapAction::FosterMcNabbTrapAction(const InputParameters & params)
  : Action(params),
    _material_definition_names(getParam<std::vector<MaterialPropertyName>>("material_definitions"))

{
    //   determine order of variables to be created 
    //    // verifyOrderAndFamilyOutputs();
    //  use displaced mesh?
    // consistency checks
}

// construct variable names for trap density and detrapping rate

void
FosterMcNabbTrapAction::act()
{
}

InputParameters
FosterMcNabbTrapAction::getKernelParameters(std::string type)
{
  InputParameters params = _factory.getValidParams(type);
  params.applyParameters(
    parameters(),
    {"displacements", "use_displaced_mesh", "save_in", "diag_save_in", "out_of_plane_strain"});

  params.set<std::vector<VariableName>>("displacements") = _coupled_displacements;

  params.set<bool>("use_displaced_mesh") = _use_displaced_mesh;

  params.set<bool>("large_kinematics") = _lk_large_kinematics;
  params.set<bool>("stabilize_strain") = _lk_locking;

  return params;
}

/*
[mobile_time_deriv]
    type = ADTimeDerivative
    variable = Mobile
  []
  [trapping_equilibrium_equation_1]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_1
    v = Mobile
    vi = V1
    n_traps = n1
    #index = 1
  []
  [trapped_time_deriv_couple_1]
    type = ADCoupledTimeDerivative 
    variable = Mobile
    v = Trapped_1
  []
  [trapped_time_deriv_1]
    type = ADTimeDerivative
    variable = Trapped_1
  []
*/

void FosterMcNabbTrapAction::addVariables(std::string names)
{
    auto params = _factory.getValidParams("MooseVariable");
    const bool second = _order_specified ? _order == "SECOND" : _problem->mesh().hasSecondOrderElements();

    params.set<MooseEnum>("order") = second_order ? "SECOND" : "FIRST";
    params.set<MooseEnum>("family") = "LAGRANGE";

    for (const auto & name : names)
    {
        _problem->addVariable("MooseVariable", name, params);
    }
}

void FosterMcNabbTrapAction::addMaterials()
{
    addArhheniousMaterials(_traps);
}

void FosterMcNabbTrapAction::addArrheniusMaterials(std::map<std::string, FosterMcNabbTrapAction::trap_parameters> & names)
{
    /*
        1) block restrictions? 
    */
    std::string type = "ArrheniusMaterial"
    for (auto const& [name, values] : names) // c++17
    {
        params = _factory.getValidParams(type);
        params.set<MaterialPropertyName>("name") = name;
        params.set<Real>("v0") = values.v0; // if we make a struct can call by name to be more explicit
        params.set<Real>("E") = values.E;
        params.set<Real>("k") = _k; //assume we don't want to mix eV and kJ/mol anyway so let's keep this constant
        params.set<NonlinearVariableName>("Temperature") = _temperature_variable;
        std::string material_block_name = name + "_material"
        _problem->addMaterial(type, material_block_name, params);
        
    }
}

void FosterMcNabbTrapAction::addArrheniusMaterials(std::map<std::string, std::pair<Real, Real> & names)
{
    /*
        1) block restrictions? 
    */
    std::string type = "ArrheniusMaterial"
    for (auto const& [name, values] : names) // c++17
    {
        params = _factory.getValidParams(type);
        params.set<MaterialPropertyName>("name") = name;
        params.set<Real>("v0") = values.first; // if we make a struct can call by name to be more explicit
        params.set<Real>("E") = values.second;
        params.set<Real>("k") = _k; //assume we don't want to mix eV and kJ/mol anyway so let's keep this constant
        params.set<NonlinearVariableName>("Temperature") = _temperature_variable;
        std::string material_block_name = name + "_material"
        _problem->addMaterial(type, material_block_name, params);
        
    }
}

void FosterMcNabbTrapAction::addTrappingKernels()
{

}

void FosterMcNabbTrapAction::addTrappingReactionKernels(std::vector<std::string>> trap_variables)
{

  
}

void FosterMcNabbTrapAction::addTimeKernels(std::vector<std::string>> variables)
{
  std::string kernel_type = "ADTimeDerivative";
  for (auto variable_name : variables)
  {
    InputParameters params = _factory.getValidParams(kernel_type);
    params.set<NonlinearVariableName>("variable") = variable_name;
    std::string kernel_name = variable_name + "_dt"
    _problem->addKernel(kernel_type, kernel_name, params);
  }
}

// maybe better to have vector< pair< string,vector<string> > > 
// as we expect multiple traps to be associated with each mobile
void FosterMcNabbTrapAction::addTrapCouplingKernels(std::vector<std::pair<std::string,std::string>> mobile_trap_pairs)
{
  std::string kernel_type = "ADCoupledTimeDerivative";
  for (auto variable_pair : mobile_trap_pairs)
  {
    std::string trap_variable_name = variable_pair.first;
    std::string mobile_variable_name = variable_pair.second;

    InputParameters params = _factory.getValidParams(kernel_type);
    params.set<NonlinearVariableName>("v") = trap_variable_name;
    params.set<NonlinearVariableName>("variable") = mobile_variable_name;
    std::string kernel_name = variable_name + "_coupled_dt"
    _problem->addKernel(kernel_type, kernel_name, params);

  }
}

// void
// TensorMechanicsAction::act()
// {
//   std::string ad_prepend = "";
//   if (_use_ad)
//     ad_prepend = "AD";

//   // Consistency checks across subdomains
//   actSubdomainChecks();

//   // Gather info from all other TensorMechanicsAction
//   actGatherActionParameters();

//   // Deal with the optional AuxVariable based tensor quantity output
//   actOutputGeneration();

//   // Meta action which optionally spawns other actions
//   if (_current_task == "meta_action")
//   {
//     if (_planar_formulation == PlanarFormulation::GeneralizedPlaneStrain)
//     {
//       if (_lagrangian_kernels)
//         paramError("Plane formulation not available with Lagrangian kernels");
//       ;

//       if (_use_ad)
//         paramError("use_automatic_differentiation", "AD not setup for use with PlaneStrain");
//       // Set the action parameters
//       const std::string type = "GeneralizedPlaneStrainAction";
//       auto action_params = _action_factory.getValidParams(type);
//       action_params.set<bool>("_built_by_moose") = true;
//       action_params.set<std::string>("registered_identifier") = "(AutoBuilt)";

//       // Skipping selected parameters in applyParameters() and then manually setting them only if
//       // they are set by the user is just to prevent both the current and deprecated variants of
//       // these parameters from both getting passed to the UserObject. Once we get rid of the
//       // deprecated versions, we can just set them all with applyParameters().
//       action_params.applyParameters(parameters(),
//                                     {"use_displaced_mesh",
//                                      "out_of_plane_pressure",
//                                      "out_of_plane_pressure_function",
//                                      "factor",
//                                      "pressure_factor"});
//       action_params.set<bool>("use_displaced_mesh") = _use_displaced_mesh;

//       if (parameters().isParamSetByUser("out_of_plane_pressure"))
//         action_params.set<FunctionName>("out_of_plane_pressure") =
//             getParam<FunctionName>("out_of_plane_pressure");
//       if (parameters().isParamSetByUser("out_of_plane_pressure_function"))
//         action_params.set<FunctionName>("out_of_plane_pressure_function") =
//             getParam<FunctionName>("out_of_plane_pressure_function");
//       if (parameters().isParamSetByUser("factor"))
//         action_params.set<Real>("factor") = getParam<Real>("factor");
//       if (parameters().isParamSetByUser("pressure_factor"))
//         action_params.set<Real>("pressure_factor") = getParam<Real>("pressure_factor");

//       // Create and add the action to the warehouse
//       auto action = MooseSharedNamespace::static_pointer_cast<MooseObjectAction>(
//           _action_factory.create(type, name() + "_gps", action_params));
//       _awh.addActionBlock(action);
//       if (isParamValid("extra_vector_tags"))
//         action_params.set<std::vector<TagName>>("extra_vector_tags") =
//             getParam<std::vector<TagName>>("extra_vector_tags");
//     }
//   }

//   // Add variables
//   else if (_current_task == "add_variable")
//   {
//     if (getParam<bool>("add_variables"))
//     {
//       auto params = _factory.getValidParams("MooseVariable");
//       // determine necessary order
//       const bool second = _problem->mesh().hasSecondOrderElements();

//       params.set<MooseEnum>("order") = second ? "SECOND" : "FIRST";
//       params.set<MooseEnum>("family") = "LAGRANGE";
//       if (isParamValid("scaling"))
//         params.set<std::vector<Real>>("scaling") = {getParam<Real>("scaling")};

//       // Loop through the displacement variables
//       for (const auto & disp : _displacements)
//       {
//         // Create displacement variables
//         _problem->addVariable("MooseVariable", disp, params);
//       }
//     }

//     // Homogenization scalar
//     if (_lk_homogenization)
//     {
//       InputParameters params = _factory.getValidParams("MooseVariable");
//       params.set<MooseEnum>("family") = "SCALAR";
//       params.set<MooseEnum>("order") =
//           HomogenizationConstants::required.at(_lk_large_kinematics)[_ndisp - 1];
//       auto fe_type = AddVariableAction::feType(params);
//       auto var_type = AddVariableAction::determineType(fe_type, 1);
//       _problem->addVariable(var_type, _hname, params);
//     }
//   }
//   // Add Materials
//   else if (_current_task == "add_master_action_material")
//   {
//     // Automatic eigenstrain names
//     if (_auto_eigenstrain)
//       actEigenstrainNames();

//     // Easiest just to branch on type here, as the strain systems are completely
//     // different
//     if (_lagrangian_kernels)
//       actLagrangianKernelStrain();
//     else
//       actStressDivergenceTensorsStrain();
//   }

//   // Add Stress Divergence (and optionally WeakPlaneStress) Kernels
//   else if (_current_task == "add_kernel")
//   {
//     for (unsigned int i = 0; i < _ndisp; ++i)
//     {
//       auto tensor_kernel_type = getKernelType();
//       auto params = getKernelParameters(ad_prepend + tensor_kernel_type);

//       std::string kernel_name = "TM_" + name() + Moose::stringify(i);

//       // Set appropriate components for kernels, including in the cases where a planar model is
//       // running in planes other than the x-y plane (defined by _out_of_plane_strain_direction).
//       if (_out_of_plane_direction == OutOfPlaneDirection::x && i == 0)
//         continue;
//       else if (_out_of_plane_direction == OutOfPlaneDirection::y && i == 1)
//         continue;

//       params.set<unsigned int>("component") = i;

//       params.set<NonlinearVariableName>("variable") = _displacements[i];

//       if (_save_in.size() == _ndisp)
//         params.set<std::vector<AuxVariableName>>("save_in") = {_save_in[i]};
//       if (_diag_save_in.size() == _ndisp)
//         params.set<std::vector<AuxVariableName>>("diag_save_in") = {_diag_save_in[i]};
//       if (isParamValid("out_of_plane_strain") && !_lagrangian_kernels)
//         params.set<std::vector<VariableName>>("out_of_plane_strain") = {
//             getParam<VariableName>("out_of_plane_strain")};

//       if (_lk_homogenization)
//       {
//         params.set<std::vector<VariableName>>("macro_gradient") = {_hname};
//         params.set<MultiMooseEnum>("constraint_types") = _constraint_types;
//       }

//       _problem->addKernel(ad_prepend + tensor_kernel_type, kernel_name, params);
//     }

//     if (_planar_formulation == PlanarFormulation::WeakPlaneStress)
//     {
//       auto params = getKernelParameters(ad_prepend + "WeakPlaneStress");
//       std::string wps_kernel_name = "TM_WPS_" + name();
//       params.set<NonlinearVariableName>("variable") = getParam<VariableName>("out_of_plane_strain");

//       _problem->addKernel(ad_prepend + "WeakPlaneStress", wps_kernel_name, params);
//     }
//   }
//   else if (_current_task == "add_scalar_kernel")
//   {
//     if (_lk_homogenization)
//     {
//       InputParameters params = _factory.getValidParams("HomogenizationConstraintScalarKernel");
//       params.set<NonlinearVariableName>("variable") = _hname;
//       params.set<unsigned int>("ndim") = _ndisp;
//       params.set<UserObjectName>("integrator") = _integrator_name;
//       params.set<bool>("large_kinematics") = _lk_large_kinematics;

//       _problem->addScalarKernel(
//           "HomogenizationConstraintScalarKernel", "HomogenizationConstraints", params);
//     }
//   }
//   else if (_current_task == "add_user_object")
//   {
//     if (_lk_homogenization)
//     {
//       InputParameters params = _factory.getValidParams("HomogenizationConstraintIntegral");
//       params.set<std::vector<VariableName>>("displacements") = _displacements;
//       params.set<MultiMooseEnum>("constraint_types") = _constraint_types;
//       params.set<std::vector<FunctionName>>("targets") = _targets;
//       params.set<bool>("large_kinematics") = _lk_large_kinematics;
//       params.set<ExecFlagEnum>("execute_on") = {EXEC_INITIAL, EXEC_LINEAR};

//       _problem->addUserObject("HomogenizationConstraintIntegral", _integrator_name, params);
//     }
//   }
// }

// void
// TensorMechanicsAction::actSubdomainChecks()
// {
//   // Do the coordinate system check only once the problem is created
//   if (_current_task == "setup_mesh_complete")
//   {
//     // get subdomain IDs
//     for (auto & name : _subdomain_names)
//       _subdomain_ids.insert(_mesh->getSubdomainID(name));
//   }

//   if (_current_task == "validate_coordinate_systems")
//   {
//     // use either block restriction list or list of all subdomains in the mesh
//     const auto & check_subdomains =
//         _subdomain_ids.empty() ? _problem->mesh().meshSubdomains() : _subdomain_ids;
//     if (check_subdomains.empty())
//       mooseError("No subdomains found");

//     // make sure all subdomains are using the same coordinate system
//     _coord_system = _problem->getCoordSystem(*check_subdomains.begin());
//     for (auto subdomain : check_subdomains)
//       if (_problem->getCoordSystem(subdomain) != _coord_system)
//         mooseError("The TensorMechanics action requires all subdomains to have the same coordinate "
//                    "system.");

//     if (_coord_system == Moose::COORD_RZ)
//     {
//       if (_out_of_plane_direction != OutOfPlaneDirection::z)
//         mooseError("'out_of_plane_direction' must be 'z' for axisymmetric simulations");
//     }
//     else if (_planar_formulation != PlanarFormulation::None)
//     {
//       if (_out_of_plane_direction == OutOfPlaneDirection::z && _ndisp != 2)
//         mooseError(
//             "Must specify two displacements for plane strain when the out of plane direction is z");
//       else if (_out_of_plane_direction != OutOfPlaneDirection::z && _ndisp != 3)
//         mooseError("Must specify three displacements for plane strain when the out of plane "
//                    "direction is x or y");
//     }
//   }
// }

// void
// TensorMechanicsAction::actOutputGeneration()
// {
//   if (_current_task == "add_material")
//     actOutputMatProp();

//   // Add variables (optional)
//   if (_current_task == "add_aux_variable")
//   {
//     unsigned int index = 0;
//     for (auto out : _generate_output)
//     {
//       const auto & order = _material_output_order[index];
//       const auto & family = _material_output_family[index];

//       std::string type = (order == "CONSTANT" && family == "MONOMIAL")
//                              ? "MooseVariableConstMonomial"
//                              : "MooseVariable";

//       // Create output helper aux variables
//       auto params = _factory.getValidParams(type);
//       params.set<MooseEnum>("order") = order;
//       params.set<MooseEnum>("family") = family;

//       if (family == "MONOMIAL")
//         _problem->addAuxVariable(type, _base_name + out, params);
//       else
//         _problem->addVariable(type, _base_name + out, params);

//       index++;
//     }
//   }

//   // Add output AuxKernels
//   else if (_current_task == "add_aux_kernel")
//   {
//     std::string ad_prepend = _use_ad ? "AD" : "";
//     // Loop through output aux variables
//     unsigned int index = 0;
//     for (auto out : _generate_output)
//     {
//       if (_material_output_family[index] == "MONOMIAL")
//       {
//         InputParameters params = emptyInputParameters();

//         params = _factory.getValidParams("MaterialRealAux");
//         params.applyParameters(parameters());
//         params.set<MaterialPropertyName>("property") = _base_name + out;
//         params.set<AuxVariableName>("variable") = _base_name + out;
//         params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;

//         _problem->addAuxKernel(
//             ad_prepend + "MaterialRealAux", _base_name + out + '_' + name(), params);
//       }
//       index++;
//     }
//   }
//   else if (_current_task == "add_kernel")
//   {
//     std::string ad_prepend = _use_ad ? "AD" : "";
//     // Loop through output aux variables
//     unsigned int index = 0;
//     for (auto out : _generate_output)
//     {
//       if (_material_output_family[index] != "MONOMIAL")
//       {
//         InputParameters params = emptyInputParameters();

//         params = _factory.getValidParams("MaterialPropertyValue");
//         params.applyParameters(parameters());
//         params.set<MaterialPropertyName>("prop_name") = _base_name + out;
//         params.set<NonlinearVariableName>("variable") = _base_name + out;

//         _problem->addKernel(
//             ad_prepend + "MaterialPropertyValue", _base_name + out + '_' + name(), params);
//       }
//       index++;
//     }
//   }
// }

// void
// TensorMechanicsAction::actEigenstrainNames()
// {
//   // Create containers for collecting blockIDs and eigenstrain names from materials
//   std::map<std::string, std::set<SubdomainID>> material_eigenstrain_map;
//   std::set<std::string> eigenstrain_set;

//   std::set<MaterialPropertyName> verified_eigenstrain_names;

//   std::map<std::string, std::string> remove_add_map;
//   std::set<std::string> remove_reduced_set;

//   // Loop over all the materials(eigenstrains) already created
//   auto materials = _problem->getMaterialWarehouse().getObjects();
//   for (auto & mat : materials)
//   {
//     std::shared_ptr<BlockRestrictable> blk = std::dynamic_pointer_cast<BlockRestrictable>(mat);
//     const InputParameters & mat_params = mat->parameters();
//     auto & mat_name = mat->type();

//     // Check for eigenstrain names, only deal with those materials
//     if (mat_params.isParamValid("eigenstrain_name"))
//     {
//       std::shared_ptr<MaterialData> mat_dat;
//       auto name = mat_params.get<std::string>("eigenstrain_name");

//       // Check for base_name prefix
//       if (mat_params.isParamValid("base_name"))
//         name = mat_params.get<std::string>("base_name") + '_' + name;

//       // Check block restrictions
//       if (!blk)
//         mooseError("Internal error, Material object that does not inherit form BlockRestricted");
//       const std::set<SubdomainID> & blocks =
//           blk->blockRestricted() ? blk->blockIDs() : blk->meshBlockIDs();

//       if (std::includes(blocks.begin(), blocks.end(), _subdomain_ids.begin(), _subdomain_ids.end()))
//       {
//         material_eigenstrain_map[name].insert(blocks.begin(), blocks.end());
//         eigenstrain_set.insert(name);
//       }
//     }

//     // Account for reduced eigenstrains and CompositeEigenstrains
//     if (mat_name == "ComputeReducedOrderEigenstrain")
//     {
//       auto input_eigenstrain_names =
//           mat_params.get<std::vector<MaterialPropertyName>>("input_eigenstrain_names");
//       remove_reduced_set.insert(input_eigenstrain_names.begin(), input_eigenstrain_names.end());
//     }
//     // Account for CompositeEigenstrains
//     if (mat_name == "CompositeEigenstrain")
//     {
//       auto remove_list = mat_params.get<std::vector<MaterialPropertyName>>("tensors");
//       for (auto i : remove_list)
//         remove_reduced_set.insert(i);
//     }

//     // Account for MaterialADConverter , add or remove later
//     if (mat_name == "RankTwoTensorMaterialADConverter")
//     {
//       std::vector<MaterialPropertyName> remove_list;
//       std::vector<MaterialPropertyName> add_list;

//       if (mat_params.isParamValid("ad_props_out") && mat_params.isParamValid("reg_props_in") &&
//           _use_ad)
//       {
//         remove_list = mat_params.get<std::vector<MaterialPropertyName>>("reg_props_in");
//         add_list = mat_params.get<std::vector<MaterialPropertyName>>("ad_props_out");
//       }
//       if (mat_params.isParamValid("ad_props_in") && mat_params.isParamValid("reg_props_out") &&
//           !_use_ad)
//       {
//         remove_list = mat_params.get<std::vector<MaterialPropertyName>>("ad_props_in");
//         add_list = mat_params.get<std::vector<MaterialPropertyName>>("reg_props_out");
//       }

//       // These vectors are the same size as checked in MaterialADConverter
//       for (unsigned int index = 0; index < remove_list.size(); index++)
//         remove_add_map.emplace(remove_list[index], add_list[index]);
//     }
//   }
//   // All the materials have been accounted for, now remove or add parts

//   // Remove names which aren't eigenstrains (converter properties)
//   for (auto remove_add_index : remove_add_map)
//   {
//     const bool is_in = eigenstrain_set.find(remove_add_index.first) != eigenstrain_set.end();
//     if (is_in)
//     {
//       eigenstrain_set.erase(remove_add_index.first);
//       eigenstrain_set.insert(remove_add_index.second);
//     }
//   }
//   for (auto index : remove_reduced_set)
//     eigenstrain_set.erase(index);

//   // Compare the blockIDs set of eigenstrain names with the vector of _eigenstrain_names for the
//   // current subdomainID
//   std::set_union(eigenstrain_set.begin(),
//                  eigenstrain_set.end(),
//                  _eigenstrain_names.begin(),
//                  _eigenstrain_names.end(),
//                  std::inserter(verified_eigenstrain_names, verified_eigenstrain_names.begin()));

//   // Ensure the eigenstrain names previously passed include any missing names
//   _eigenstrain_names.resize(verified_eigenstrain_names.size());
//   std::copy(verified_eigenstrain_names.begin(),
//             verified_eigenstrain_names.end(),
//             _eigenstrain_names.begin());

//   Moose::out << COLOR_CYAN << "*** Automatic Eigenstrain Names ***"
//              << "\n"
//              << _name << ": " << Moose::stringify(_eigenstrain_names) << "\n"
//              << COLOR_DEFAULT << std::flush;
// }

// void
// TensorMechanicsAction::verifyOrderAndFamilyOutputs()
// {
//   // Ensure material output order and family vectors are same size as generate output

//   // check number of supplied orders and families
//   if (_material_output_order.size() > 1 && _material_output_order.size() < _generate_output.size())
//     paramError("material_output_order",
//                "The number of orders assigned to material outputs must be: 0 to be assigned "
//                "CONSTANT; 1 to assign all outputs the same value, or the same size as the number "
//                "of generate outputs listed.");

//   if (_material_output_family.size() > 1 &&
//       _material_output_family.size() < _generate_output.size())
//     paramError("material_output_family",
//                "The number of families assigned to material outputs must be: 0 to be assigned "
//                "MONOMIAL; 1 to assign all outputs the same value, or the same size as the number "
//                "of generate outputs listed.");

//   // if no value was provided, chose the default CONSTANT
//   if (_material_output_order.size() == 0)
//     _material_output_order.push_back("CONSTANT");

//   // For only one order, make all orders the same magnitude
//   if (_material_output_order.size() == 1)
//     _material_output_order =
//         std::vector<std::string>(_generate_output.size(), _material_output_order[0]);

//   if (_verbose)
//     Moose::out << COLOR_CYAN << "*** Automatic applied material output orders ***"
//                << "\n"
//                << _name << ": " << Moose::stringify(_material_output_order) << "\n"
//                << COLOR_DEFAULT << std::flush;

//   // if no value was provided, chose the default MONOMIAL
//   if (_material_output_family.size() == 0)
//     _material_output_family.push_back("MONOMIAL");

//   // For only one family, make all families that value
//   if (_material_output_family.size() == 1)
//     _material_output_family =
//         std::vector<std::string>(_generate_output.size(), _material_output_family[0]);

//   if (_verbose)
//     Moose::out << COLOR_CYAN << "*** Automatic applied material output families ***"
//                << "\n"
//                << _name << ": " << Moose::stringify(_material_output_family) << "\n"
//                << COLOR_DEFAULT << std::flush;
// }

// void
// TensorMechanicsAction::actGatherActionParameters()
// {
//   // Gather info about all other master actions when we add variables
//   if (_current_task == "validate_coordinate_systems" && getParam<bool>("add_variables"))
//   {
//     auto actions = _awh.getActions<TensorMechanicsAction>();
//     for (const auto & action : actions)
//     {
//       const auto size_before = _subdomain_id_union.size();
//       const auto added_size = action->_subdomain_ids.size();
//       _subdomain_id_union.insert(action->_subdomain_ids.begin(), action->_subdomain_ids.end());
//       const auto size_after = _subdomain_id_union.size();

//       if (size_after != size_before + added_size)
//         mooseError("The block restrictions in the TensorMechanics/Master actions must be "
//                    "non-overlapping.");

//       if (added_size == 0 && actions.size() > 1)
//         mooseError("No TensorMechanics/Master action can be block unrestricted if more than one "
//                    "TensorMechanics/Master action is specified.");
//     }
//   }
// }


