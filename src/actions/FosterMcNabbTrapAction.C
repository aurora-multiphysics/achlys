// FosterMcNabbTrapAction
// "achlysApp"
#include "Conversion.h"
#include "FEProblem.h"
#include "Factory.h"
#include "MooseMesh.h"
#include "MooseObjectAction.h"
#include "FosterMcNabbTrapAction.h"
#include "Material.h"

#include "BlockRestrictable.h"

// #include "HomogenizationConstraintIntegral.h" // just for the constants
#include "AddVariableAction.h"

#include "libmesh/string_to_enum.h"
#include <algorithm>
#include <iostream>


registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_variable");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_kernel");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_material");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_interface_kernel");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_aux_variable");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_aux_kernel");


/*
    1) create new trap_i variables for each element given in material property vectors (for each material subdomain)
        i) consider variable order and family

    2) declare new material properties 

    3) create 3 kernels for the Foster-McNabb trapping model, including coupling with the mobile term
        i) test if simulation is steady or transient to optionally create time-based kernels

    4) instantiate interface kernels if required

    5) create summary aux-variables which summarise global mobile / trapped HI in SI units
        i) option to disable if requested

    Questions:
    i) create mobile variables here too? -- how will the user-object ecosystem look?
    ii) global variable options? (seen some convergence issues just being block-restricted)
    iii) can this generalise to multi-isotope models?
    iv) switch between molar and atomic units? 
    v) stateful PJFNK? set_mf_reuse_base(false)

    -- create new data strcutures or just use naive loops through input variables to start?

    -- BCs etc. will wlays be in terms of mobile species so maybe best to set these explicitly outsid eof the action?

    To-do:
      - add ENUMS for some input options:
        i) variable order
        ii) interface models
        iii) energy units

      - switches
        i) create variable or use existing
        ii) create summary aux-variables

      - automate unit rescaling from SI input to atomic fraction in calc
        i) options for energy units

      - add interface kernels
        i) need a common (or final) action to grab list of boundary ids from blocks implementing this action

      - add aux kernel summary

      - option for function input for trap density instead of domain constant value
        i) using parsed material could have a list input where entries are either constant values
           or functions names. or 2 lists, constants and variable
        ii) accept list of aux variable names for trap densities controlled by other functions

      - option for constant temperature instead of aux variable

      - handle options for accepting existing variable rather than creating new obes

      - solubility can be an interface material?

      - full-coverage tests

*/

InputParameters
FosterMcNabbTrapAction::validParams()
{
    InputParameters params = DiffusiveMaterialAction::validParams();
    params.addClassDescription("Set up kernels and materials for a Foster-McNabb trapping model");
    params.addRequiredParam<std::vector<Real>>("v0", "pre-exponential detrapping factor in Arrhenious eq.");
    params.addRequiredParam<std::vector<Real>>("E", "Trap detrapping energy in eV");
    params.addRequiredParam<std::vector<Real>>("n", "possible trapping sites");
    params.addParam<Real>("p0", -1, "Explicit pre-exponential factor for the trapping reaction rate (bypasses internal calculaution based on D(T) and lambda)");
    params.addParam<Real>("Ep", -1, "Explicit binding energy for the trapping reaction rate");
    params.addParam<Real>("lambda", -1, "Lattice constant in m-1");
    params.addParam<Real>("n_sol", -1, "density of interstitial sites in atomic fraction");
    params.addParam<std::string>("trap_variable_base", "trap", "the base name for automatically created trap variables");
    params.addParam<std::string>("detrap_material_base", "detrapping_rate", "the base name for the de-trapping rate material property");
    params.addParam<std::string>("trapping_material_base", "trapping_rate", "the base name for the trapping rate material property");
    params.addParam<std::string>("trap_density_material_base", "trap_density", "the base name for trap density material property");
    // params.addParam<std::string>("jsonify", "", "path to write json data to");
    // enum for order of variables
    // enum for molar or eV formulation
    // handle variable trap densities 
    // varying requirements for input variables e.g. solubility for CP interface but not otherwise
    // options to write created aux variables/materials to file
    return params;
}

FosterMcNabbTrapAction::FosterMcNabbTrapAction(const InputParameters & params)
  : DiffusiveMaterialAction(params),
//   BlockRestrictable(this),
    // _material_definition_names(getParam<std::vector<MaterialPropertyName>>("material_definitions"))
    _n(getParam<std::vector<Real>>("n")),
    _v0(getParam<std::vector<Real>>("v0")),
    _E(getParam<std::vector<Real>>("E")),
    _p0(getParam<Real>("p0")),
    _Ep(getParam<Real>("Ep")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _trap_variable_base(getParam<std::string>("trap_variable_base")),
    _detrap_material_base(getParam<std::string>("detrap_material_base")),
    _trap_material_base(getParam<std::string>("trapping_material_base")),
    _trap_density_material_base(getParam<std::string>("trap_density_material_base"))
{
    //   determine order of variables to be created 
    //    // verifyOrderAndFamilyOutputs();
    //  use displaced mesh?
    // consistency checks
    
    bool equal_arrays = _v0.size() == _n.size() && _E.size() == _n.size();
    if (! equal_arrays)
    {
        mooseError("The trap parameter vectors v0, ni, and Ei must be the same length");
    }
    else
    {
        _n_traps = _v0.size();
    }

    _trapping_rate_specified = params.isParamSetByUser("p0");
    _trapping_energy_specified = params.isParamSetByUser("Ep");
    if (!_trapping_rate_specified)
    {
       bool required_params_specified = params.isParamSetByUser("lambda") && params.isParamSetByUser("n_sol");
       if (!required_params_specified)
       {
           mooseError("Trapping reaction rate must either be specified using (i) the lattice spacing parameter, lambda, "
            "and the number of solute sites per lattice atom, n_sol, or (ii) explicitly setting the trapping rate pre-exponential "
            "factor, p0.");
       }
    }
    else if (!_trapping_energy_specified)
    {
        _Ep = _Ed;
    }

    for (int i = 0; i < _n_traps; i++)
    {
        std::string name = _trap_variable_base + _block_prepend + "_" + std::to_string(i +1);
        _trap_variable_names.push_back(name);

        std::string trap_densityty_name = _trap_density_material_base + _block_prepend + "_" + std::to_string(i +1);
        _trap_density_names.push_back(trap_densityty_name);

        std::string detrappign_rate_name = _detrap_material_base + _block_prepend + "_" + std::to_string(i +1);
        _detrapping_rate_names.push_back(detrappign_rate_name);
    }

    _all_variable_names =  _trap_variable_names;
    _all_variable_names.push_back(_mobile_variable_name);

    _trapping_rate_material_name = _trap_material_base + _block_prepend;

    // _transient = _problem->isTransient();
    // bool naive_interface = _interface_type == InterfaceType::concentration; 
    
    //  if (!isParamValid("variable_order"))
    // {
    //     // const bool second_order =  _mesh.hasSecondOrderElements();
    //     _variable_order = "FIRST"; //second_order ? "SECOND" : "FIRST";
    // }
    // }

    // if (isParamValid("solid_boundaries"))
    // {
    //     _solid_boundaries = getParam<std::vector<std::string>>("solid_boundaries");
    // }
 
    // _k = (_energy_units == EnergyUnits::eV) ? AchlysConstants::Boltzmann : AchlysConstants::UiversalGas;
    std::vector<std::string> request = getParam<std::vector<std::string>>("aux_variables");
    if (std::find(request.begin(), request.end(), "trapped") != request.end())
            {
                _aux_variable_names.push_back("trapped");
            }

    if(params.isParamSetByUser("jsonify")){
        jsonify(getParam<std::string>("jsonify"));
    }

}


void
FosterMcNabbTrapAction::act()
{
    // if (_current_task == "setup_mesh_complete")
    // {
    //     if (!_variable_order_specified)
    //     {
    //         const bool second_order =  _mesh->hasSecondOrderElements();
    //         _variable_order = second_order ? "SECOND" : "FIRST";
    //     }
    // }
    if (_current_task == "add_variable")
    {
        addVariables();
    }
    if (_current_task == "add_material")
    {
        addMaterials();
    }
    if (_current_task == "add_kernel")
    {
        addKernels();
    }
    if (_current_task == "add_aux_variable" && !_aux_variable_names.empty())
    {
        addAuxVariables();
    }
    if (_current_task == "add_aux_kernel" && !_aux_variable_names.empty())
    {
        addAuxKernels();
    }
    if (_current_task == "add_interface_kernel" && !_solid_boundaries.empty())
    {
        addInterfaceKernels();
    }
}


// void FosterMcNabbTrapAction::addVariables()
// {
//     auto params = _factory.getValidParams("MooseVariable");
//     // const bool second = _order_specified ? _order == "SECOND" : _problem->mesh().hasSecondOrderElements();
//     // const bool second_order =  _problem->mesh().hasSecondOrderElements();
//     // params.set<MooseEnum>("order") = second_order ? "SECOND" : "FIRST";
//     params.set<MooseEnum>("order") = _variable_order;
//     params.set<MooseEnum>("family") = "LAGRANGE";
//     if (!_blocks.empty())
//     {
//         params.set<std::vector<SubdomainName>>("block") = _blocks; 
//     }

//     for (const auto & name : _all_variable_names)
//     {
//         _problem->addVariable("MooseVariable", name, params);
//     }
// }

void FosterMcNabbTrapAction::addMaterials()
{
    // addArrheniousMaterials(_traps); // could still use vector material here possibly...
    addArrheniusMaterial("D", _D0, _Ed);
    if (_solubility_specified) addArrheniusMaterial("S", _S0, _Es);

    if (_trapping_rate_specified)
    {
        addArrheniusMaterial(_trapping_rate_material_name, _p0, _Ep);
    }
    else
    {
        Real p0 = _D0 / ( std::pow(_lambda, 2) * _n_sol);
        addArrheniusMaterial(_trapping_rate_material_name, p0, _Ed);
    }

    addDetrappingRateMaterials();
    addGenericConstantMaterial(_trap_density_names, _n);
    addGenericConstantMaterial({"rho"}, {_rho});
    
}

void FosterMcNabbTrapAction::addDetrappingRateMaterials()
{
    std::string type = "ArrheniusMaterial";
    for (int i = 0; i< _n_traps; i++)
    {
        auto params = _factory.getValidParams(type);
        std::string material_property_name = _detrapping_rate_names[i]; 
        //_detrap_material_base + "_" + std::to_string(i + 1);
        // params.set<MaterialPropertyName>("name") = material_property_name;
        params.set<std::string>("name") = material_property_name;
        params.set<Real>("v0") = _v0[i];
        params.set<Real>("E") = _E[i];
        params.set<Real>("k") = _k;
        // params.set<NonlinearVariableName>("Temperature") = _temperature_variable;
        params.set<std::vector<VariableName>>("Temperature") = {_temperature_variable};
        std::string material_block_name = material_property_name + "_material" + _block_prepend;
        if (!_blocks.empty())
        {
            params.set<std::vector<SubdomainName>>("block") = _blocks; 
        }
        if (_exodus) params.set<std::vector<OutputName>>("outputs") = {"exodus"}; 
        _problem->addMaterial(type, material_block_name, params);
    }
}

void FosterMcNabbTrapAction::addParsedMaterial(std::string name, std::vector<std::string> args, std::string function)
{
    std::string type = "ADParsedMaterial";
    auto params = _factory.getValidParams(type);
    std::string f_name = name;
    params.set<std::string>("f_name") = f_name;
    // params.set<std::vector<std::string>>("args") = args;
    params.set<std::string>("function") = function;
    std::string material_block_name = name + "parsed_material" + _block_prepend;
    _problem->addMaterial(type, material_block_name, params);

}

void FosterMcNabbTrapAction::addKernels()
{
    addTrappingReactionKernels();

    if (_problem->isTransient())
    {
        addTimeKernels();
        addTrapCouplingKernels();
    }

    addDiffusionKernel();
}

void FosterMcNabbTrapAction::addTrappingReactionKernels()
{
    std::string kernel_type = "ADTrappingEquilibriumEquation";
    for (int i = 0; i < _n_traps; i++)
    {
        InputParameters params = _factory.getValidParams(kernel_type);
        params.set<std::vector<VariableName>>("v") = {_mobile_variable_name};
        params.set<NonlinearVariableName>("variable") = _trap_variable_names[i];
        std::string kernel_name = _trap_variable_names[i] + "_trapping_reaction";
        params.set<MaterialPropertyName>("vi") = _detrapping_rate_names[i];
        params.set<MaterialPropertyName>("n_traps") = _trap_density_names[i];
        params.set<MaterialPropertyName>("Vm") = {_trapping_rate_material_name};
        _problem->addKernel(kernel_type, kernel_name, params);
    }
}

void FosterMcNabbTrapAction::addTimeKernels()
{
  std::string kernel_type = "ADTimeDerivative";
  for (auto variable_name : _all_variable_names)
  {
    InputParameters params = _factory.getValidParams(kernel_type);
    params.set<NonlinearVariableName>("variable") = variable_name;
    std::string kernel_name = variable_name + "_dt";    
    _problem->addKernel(kernel_type, kernel_name, params);
  }
}

void FosterMcNabbTrapAction::addTrapCouplingKernels()
{
    std::string kernel_type = "ADCoupledTimeDerivative";
  for (int i = 0; i < _n_traps; i++)
    {
        InputParameters params = _factory.getValidParams(kernel_type);
        params.set<std::vector<VariableName>>("v") = {_trap_variable_names[i]};
        params.set<NonlinearVariableName>("variable") = _mobile_variable_name;
        std::string kernel_name = _trap_variable_names[i] + "_coupled_dt";
        _problem->addKernel(kernel_type, kernel_name, params);
    }
}

// void FosterMcNabbTrapAction::addAuxVariables()
// {
//     for (auto variable_name: _aux_variable_names)
//     {
//         add_aux_variable(variable_name);
//     }
// }

void FosterMcNabbTrapAction::addAuxKernels()
{   
    // could add map of input names to outputs e.g. continuous_mobile, total_trapped, and retention
    if ( std::find(_aux_variable_names.begin(), _aux_variable_names.end(), "mobile") != _aux_variable_names.end() )
    {
        add_continuous_mobile_aux();
    }
        if ( std::find(_aux_variable_names.begin(), _aux_variable_names.end(), "trapped") != _aux_variable_names.end() )
    {
        add_total_trapped_aux();
    }    
    if ( std::find(_aux_variable_names.begin(), _aux_variable_names.end(), "retention") != _aux_variable_names.end() )
    {
        add_total_retention_aux();
    }
}

void FosterMcNabbTrapAction::add_total_trapped_aux()
{
    std::vector<std::string> args = _trap_variable_names;
    // args.push_back("rho");
    // std::vector<std::string> const_vars = {"rho"};
    // std::vector<std::string> const_vals = {std::to_string(_rho)};
    std::string function = std::string("(") + _trap_variable_names[0];
    for (auto trap: _trap_variable_names)
    {
        if (trap == _trap_variable_names[0])
        {
            continue;
        }
        function += " + " + trap;
    }
    function += ") * " + std::to_string(_rho);
    add_parsed_aux("trapped", args, function);
}

void FosterMcNabbTrapAction::add_total_retention_aux()
{
    // validate if mobile is included
    std::vector<std::string> args = _all_variable_names;
    // args.push_back("rho");
    // std::vector<std::string> const_vars = {"rho"};
    // std::vector<std::string> const_vals = {std::to_string(_rho)};
    std::string function = std::string("(") + _mobile_variable_name;
    for (auto trap: _trap_variable_names)
    {
        function += " + " + trap;
    }
    function += ") * " + std::to_string(_rho);
    add_parsed_aux("retention", args, function);
}

// void FosterMcNabbTrapAction::add_parsed_aux(std::string name, std::vector<std::string> args, std::string function)
// {
//     std::vector<VariableName> coupled_vars;
//     for (auto coupled_name: args)
//     {
//         coupled_vars.push_back(coupled_name);
//     }
//     std::string type = "ParsedAux";
//     auto params = _factory.getValidParams(type);
//     params.set<AuxVariableName>("variable") = name;
//     params.set<std::vector<VariableName>>("args") = coupled_vars; // <VariableVlaue *>
//     // params.set<std::vector<std::string>>("constant_names") = const_vars;
//     // params.set<std::vector<std::string>>("constant_expressions") = const_vals;
//     params.set<std::string>("function") = function;
//     params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
//     if (!_blocks.empty())
//         {
//             params.set<std::vector<SubdomainName>>("block") = _blocks; 
//         }
//     std::string block_name = name + _block_prepend + "_parsed_aux";
//     _problem->addAuxKernel(type, block_name, params);
// }
