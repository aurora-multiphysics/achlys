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


registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_variables");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_kernels");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_materials");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_interface_kernels");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_aux_variables");
registerMooseAction("achlysApp", FosterMcNabbTrapAction, "add_aux_kernels");


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

      - option for constant temperature instead of aux variable

      - handle options for accepting existing variable rather than creating new obes

*/

InputParameters
FosterMcNabbTrapAction::validParams()
{
    InputParameters params = Action::validParams();
    params.addClassDescription("Set up kernels and materials for a Foster-McNabb trapping model");
    params.addRequiredParam<std::vector<Real>>("v0", "pre-exponential detrapping factor in Arrhenious eq.");
    params.addRequiredParam<std::vector<Real>>("E", "Trap detrapping energy in eV");
    params.addRequiredParam<std::vector<Real>>("n", "possible trapping sites");
    params.addRequiredParam<Real>("k", "Boltzman constant");
    params.addRequiredParam<Real>("D0", "The diffusion pre-exponential factor");
    params.addRequiredParam<Real>("Ed", "diffusion energy in eV");
    params.addParam<Real>("S0", 0.0, "The solubility pre-exponential factor");
    params.addParam<Real>("Es", 0.0, "energy of solution in eV");
    params.addParam<Real>("p0", -1, "Explicit pre-exponential factor for the trapping reaction rate (bypasses internal calculaution based on D(T) and lambda)");
    params.addParam<Real>("Ep", -1, "Explicit binding energy for the trapping reaction rate");
    params.addParam<Real>("lambda", -1, "Lattice constant in m-1");
    params.addParam<Real>("n_sol", -1, "density of interstitial sites in atomic fraction");
    params.addParam<Real>("atomic_density", 1.0, "Number density of solute atoms in m^-3");
    params.addRequiredParam<std::string>("Temperature","simulation temperature");
    params.addParam<std::string>("trap_variable_base", "trap", "the base name for automatically created trap variables");
    params.addParam<std::string>("mobile_variable_base", "mobile", "the base name for automatically created mobile variables");
    params.addParam<std::string>("detrap_material_base", "detrapping_rate", "the base name for the de-trapping rate material property");
    params.addParam<std::string>("trapping_material_base", "trapping_rate", "the base name for the trapping rate material property");
    params.addParam<std::string>("trap_density_material_base", "trap_density", "the base name for trap density material property");
    params.addParam<std::string>("diffusivity_material_base", "D", "the base name for the diffusivity material property");
    params.addParam<std::string>("solubility_material_base", "S", "the base name for the solubility material property");
    params.addParam<std::vector<SubdomainName>>("block", "optional list of subdomain IDs this action applies to");
    params.addParam<MooseEnum>("variable_order",
                                  FosterMcNabbTrapAction::VariableOrders(),
                                  "Order of the variables which will be generated");
    MooseEnum interface_type("CHEMICAL_POTENTIAL CONCENTRATION", "CHEMICAL_POTENTIAL");
    params.addParam<MooseEnum>("interface_type", interface_type, "Whether to implement continuity of mobile concentration"
                                    " or continuity of chemical potential at material interfaces");
    params.addParam<std::vector<std::string>>("solid_boundaries", "List of boundaries across which to add interface kernels");
    params.addParam<bool>("generate_interface_kernels", false, "Wether to generate interface kernels between different subdomains");
    params.addParam<std::vector<std::string>>("aux_variables", "List of summary quantities which are continuous across subdomained "
                                            "and converted to SI units. Available options are mobile, trapped, and retention");
    
    // enum for order of variables
    // enum for molar or eV formulation
    // handle variable trap densities 
    // varying requirements for input variables e.g. solubility for CP interface but not otherwise
    // options to write created aux variables/materials to file
    return params;
}

FosterMcNabbTrapAction::FosterMcNabbTrapAction(const InputParameters & params)
  : Action(params),
//   BlockRestrictable(this),
    // _material_definition_names(getParam<std::vector<MaterialPropertyName>>("material_definitions"))
    _n(getParam<std::vector<Real>>("n")),
    _v0(getParam<std::vector<Real>>("v0")),
    _E(getParam<std::vector<Real>>("E")),
    _temperature_variable(getParam<std::string>("Temperature")),
    _k(getParam<Real>("k")),
    _D0(getParam<Real>("D0")),
    _Ed(getParam<Real>("Ed")),
    _S0(getParam<Real>("S0")),
    _Es(getParam<Real>("Es")),
    _p0(getParam<Real>("p0")),
    _Ep(getParam<Real>("Ep")),
    _lambda(getParam<Real>("lambda")),
    _n_sol(getParam<Real>("n_sol")),
    _rho(getParam<Real>("atomic_density")),
    _trap_variable_base(getParam<std::string>("trap_variable_base")),
    _mobile_variable_base(getParam<std::string>("mobile_variable_base")),
    _detrap_material_base(getParam<std::string>("detrap_material_base")),
    _trap_material_base(getParam<std::string>("trapping_material_base")),
    _trap_density_material_base(getParam<std::string>("trap_density_material_base")),
    _diffusivity_material_base(getParam<std::string>("diffusivity_material_base")),
    _solubility_material_base(getParam<std::string>("solubility_material_base")),
    _blocks(getParam<std::vector<SubdomainName>>("block")),
    _interface_type(getParam<MooseEnum>("interface_type").getEnum<InterfaceType>()),
    _variable_order(getParam<MooseEnum>("variable_order"))
    
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
    _solubility_specified = params.isParamSetByUser("S0") &&params.isParamSetByUser("Es");

    /*
        construct unique lists of variable names
    */
    _block_prepend = "";
    if(! _blocks.empty())
    {
        _block_prepend = std::string("_") + std::string(_blocks[0]);
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
    _mobile_variable_name = _mobile_variable_base + _block_prepend;
    _all_variable_names =  _trap_variable_names;
    _all_variable_names.push_back(_mobile_variable_name);

    _trapping_rate_material_name = _trap_material_base + _block_prepend;
    _diffusivity_material_name = _diffusivity_material_base + _block_prepend;
    _solubility_material_name = _solubility_material_base + _block_prepend;

    // uncontrolled names for generated objects:
    // - diffusivity - D
    // - Solubility - S
    // - Density - rho

    // _transient = _problem->isTransient();
    // bool naive_interface = _interface_type == InterfaceType::concentration; 
    _variable_order_specified = params.isParamSetByUser("variable_order");
    if (! _variable_order_specified)
    {
        _variable_order = "FIRST";
    }
    
    //  if (!isParamValid("variable_order"))
    // {
    //     // const bool second_order =  _mesh.hasSecondOrderElements();
    //     _variable_order = "FIRST"; //second_order ? "SECOND" : "FIRST";
    // }

    if (isParamValid("aux_variables"))
    {
        std::vector<std::string> request = getParam<std::vector<std::string>>("aux_variables");
        std::vector<std::string> permitted = {"mobile", "trapped", "retention"};
        for (auto var: permitted)
        {
            if (std::find(request.begin(), request.end(), var) != request.end())
            {
                _aux_variable_names.push_back(var);
            }
        }
        // _aux_variable_names = getParam<std::vector<std::string>>("aux_variables");
    }

    if (isParamValid("solid_boundaries"))
    {
        _solid_boundaries = getParam<std::vector<std::string>>("solid_boundaries");
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

    if (_current_task == "add_variables")
    {
        addVariables();
    }
    if (_current_task == "add_materials")
    {
        addMaterials();
    }
    if (_current_task == "add_kernels")
    {
        addKernels();
    }
    if (_current_task == "add_aux_variables" && !_aux_variable_names.empty())
    {
        addAuxVariables();
    }
    if (_current_task == "add_aux_kernels" && !_aux_variable_names.empty())
    {
        addAuxKernels();
    }
    if (_current_task == "add_interface_kernels" && !_solid_boundaries.empty())
    {
        addInterfaceKernels();
    }
}


void FosterMcNabbTrapAction::addVariables()
{
    auto params = _factory.getValidParams("MooseVariable");
    // const bool second = _order_specified ? _order == "SECOND" : _problem->mesh().hasSecondOrderElements();
    // const bool second_order =  _problem->mesh().hasSecondOrderElements();
    // params.set<MooseEnum>("order") = second_order ? "SECOND" : "FIRST";
    params.set<MooseEnum>("order") = _variable_order;
    params.set<MooseEnum>("family") = "LAGRANGE";
    if (!_blocks.empty())
    {
        params.set<std::vector<SubdomainName>>("block") = _blocks; 
    }

    for (const auto & name : _all_variable_names)
    {
        _problem->addVariable("MooseVariable", name, params);
    }
}

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
        params.set<std::vector<OutputName>>("outputs") = {"exodus"}; 
        _problem->addMaterial(type, material_block_name, params);
    }
}

void FosterMcNabbTrapAction::addArrheniusMaterial(std::string name, Real V0, Real E)
{
    std::string type = "ArrheniusMaterial";
    auto params = _factory.getValidParams(type);
    if (!_blocks.empty())
    {
        params.set<std::vector<SubdomainName>>("block") = _blocks; 
    }
    // params.set<MaterialPropertyName>("name") = name;
    params.set<std::string>("name") = name;
    params.set<Real>("v0") = V0;
    params.set<Real>("E") = E;
    params.set<Real>("k") = _k; 
    params.set<std::vector<VariableName>>("Temperature") = {_temperature_variable};
    // params.set<std::string>("Temperature") = _temperature_variable;
    params.set<std::vector<OutputName>>("outputs") = {"exodus"}; 
    std::string material_block_name = name + "_material" + _block_prepend;
    _problem->addMaterial(type, material_block_name, params);

}

void FosterMcNabbTrapAction::addGenericConstantMaterial(std::vector<std::string> names, std::vector<Real> values)
{
    std::string type = "ADGenericConstantMaterial";
    auto params = _factory.getValidParams(type);
    params.set<std::vector<std::string>>("prop_names") = names;
    params.set<std::vector<Real>>("prop_values") = values;
    std::string material_block_name = names[0] + "_material" + _block_prepend;
    if (!_blocks.empty())
    {
        params.set<std::vector<SubdomainName>>("block") = _blocks; 
    }
    params.set<std::vector<OutputName>>("outputs") = {"exodus"}; 
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

void FosterMcNabbTrapAction::addDiffusionKernel()
{
    std::string kernel_type = "ADMatDiffusion";
    InputParameters params = _factory.getValidParams(kernel_type);
    params.set<NonlinearVariableName>("variable") = _mobile_variable_name;
    params.set<MaterialPropertyName>("diffusivity") = "D";
    std::string kernel_name = _mobile_variable_name + "diffusion";
    _problem->addKernel(kernel_type, kernel_name, params);
}

void FosterMcNabbTrapAction::addAuxVariables()
{
    for (auto variable_name: _aux_variable_names)
    {
        add_aux_variable(variable_name);
    }
}


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


void FosterMcNabbTrapAction::add_aux_variable(std::string name)
{
    // only want to call this
    auto params = _factory.getValidParams("MooseVariable");
    params.set<MooseEnum>("order") = _variable_order;
    params.set<MooseEnum>("family") = "LAGRANGE";
    _problem->addAuxVariable("MooseVariable", name, params);
}

void FosterMcNabbTrapAction::add_continuous_mobile_aux()
{
    std::vector<std::string> args = {_mobile_variable_name};
    // std::vector<std::string> const_vars = {"rho"};
    // std::vector<std::string> const_vals = {std::to_string(_rho)};
    std::string function = _mobile_variable_name + " * " + std::to_string(_rho);
    std::cout << "parsed aux function: \n" << function << "\n";
    add_parsed_aux("mobile", args, function);
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


void FosterMcNabbTrapAction::add_parsed_aux(std::string name, std::vector<std::string> args, std::string function)
{
    std::vector<VariableName> coupled_vars;
    for (auto coupled_name: args)
    {
        coupled_vars.push_back(coupled_name);
    }
    std::string type = "ParsedAux";
    auto params = _factory.getValidParams(type);
    params.set<AuxVariableName>("variable") = name;
    params.set<std::vector<VariableName>>("args") = coupled_vars; // <VariableVlaue *>
    // params.set<std::vector<std::string>>("constant_names") = const_vars;
    // params.set<std::vector<std::string>>("constant_expressions") = const_vals;
    params.set<std::string>("function") = function;
    params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
    if (!_blocks.empty())
        {
            params.set<std::vector<SubdomainName>>("block") = _blocks; 
        }
    std::string block_name = name + _block_prepend + "_parsed_aux";
    _problem->addAuxKernel(type, block_name, params);
}

void FosterMcNabbTrapAction::addInterfaceKernels()
{
    // convert input array to set
    // std::set<std::string> bounday_set(_solid_boundaries.begin(), _solid_boundaries.end());

    // get list of blocks
    // _problem->hasBlockMaterialProperty
    // _problem->hasBoundaryMaterialProperty()
    // _problem->getVariableNames
    // _problem->getMaterialPropertyBoundaryNames()
    // std::set blocks = _problem->getBoundaryConnectedBlocks
    // std::set ids = _problem->getSubdomainInterfaceBoundaryIds 

    // 1. get boundary ids

    // 2. get blocks on other side of boundary

    // 3. infer (or accept input for) neighbour mobile variable name

    if (_interface_type == InterfaceType::chemical_potential)
    {
        add_chemical_potential_based_interface();
    }
    else
    {
        add_concentration_based_interface();
    }
}

void FosterMcNabbTrapAction::add_chemical_potential_based_interface()
{
    for (auto boundary_name: _solid_boundaries)
    {
        std::string neighbour_block = get_neighbour_block_name(boundary_name);
        if (!neighbour_block.empty())
        {
            std::string var2 = _mobile_variable_base + "_" + neighbour_block;
            std::cout << "chemical_potential_based block " << _blocks[0] << ": " << var2 << "\n";
            add_chemical_potential_interface(var2, _mobile_variable_name, boundary_name);
            add_mass_continuity_interface(var2, _mobile_variable_name, boundary_name);
        }
    }
}

void FosterMcNabbTrapAction::add_concentration_based_interface()
{
    for (auto boundary_name: _solid_boundaries)
    {

        std::string neighbour_block = get_neighbour_block_name(boundary_name);
        if (!neighbour_block.empty())
        {
            std::string var2 = _mobile_variable_base + "_" + get_neighbour_block_name(boundary_name);
            std::cout << "concentration_based block " << _blocks[0] << ": " << var2 << "\n";
            add_mobile_concentration_interface(_mobile_variable_name, var2, boundary_name);
            add_mass_continuity_interface(_mobile_variable_name, var2, boundary_name);
        }
    }
}

SubdomainName FosterMcNabbTrapAction::get_neighbour_block_name(std::string boundary_name)
{
    
    // 1. get list of blocks associated with boundary
    BoundaryID boundary_id = _mesh->getBoundaryID(boundary_name);
    auto block_ids = _mesh->getBoundaryConnectedBlocks(boundary_id);

    // 2. determine which block NOT is not a member of the current object
    for (auto block_id: block_ids)
    {
        SubdomainName block_name = _mesh->getSubdomainName(block_id);
        if (block_name.empty())
        {
            block_name = std::to_string(block_id);
        }
        if (std::find(_blocks.begin(), _blocks.end(), block_name) == _blocks.end())
        {
            // assumes only one
            return block_name;
        }
    }
    // error: nothing found
    return "";

    
}

/*
    Check required materials have been initialised on all blocks on a boundary before addAuxKernel
    decalring the interface kernels which required them

    - is this guaranteed if all add_kernel and add_material actions are complete before add_interface_kernels is executed?
// */
// void boundary_materials_exist(BoundaryName boundary, std::string material_name)
// {
//     //  consider hasActiveBlockObjects
//     auto boundary_id = _problem->mesh().getBoundaryID(boundary);
//     std::set<SubdomainID> blocks = _problem->mesh().getBoundaryConnectedBlocks(boundary_id);

//     std::vector<bool> required_material_exists;
//     required_material_exists.resize(blocks.size())
//     unsigned int i = 0;
//     for (auto block: blocks)
//     {
//         required_material_exists[i] = false;
//         // vector of shared pointers
//         auto objects = _problem->getMaterialWarehouse().getBlockObjects(block);
//         for (auto object: objects)
//         {
//             if (object->name() == material_name)
//             {
//                 required_material_exists[i] = true;
//                 break;
//             }
//         }
//         i++;
//     }
//     // or vector of ints and sum?
//     return std::all_of(required_material_exists.begin(), required_material_exists.end(), [](bool v) { return v; });
// }

void boundary_materials_exist(BoundaryName boundary, std::string material_name)
{
    // auto boundaries = _problem->getMaterialPropertyBoundaryNames(material_name);
}

/*
    Assuming we have one action instance for each material block then 2 blocks, 1 on each side of the interface
    will try to intialise the interface kernels. 

    Implement some method to test and prevent the second instantiation.

    -- do we also need to test for the materials on the second side are availabnle when we 
       declare the kernel on the primary side?
*/
bool FosterMcNabbTrapAction::interface_exists_already(std::string block_name)
{
    auto kernels = _problem->getNonlinearSystem().getKernelWarehouse();
    // not active!
    // return kernel.hasActiveObject(block_name);
    return false;
}

void FosterMcNabbTrapAction::add_chemical_potential_interface(std::string variable_1_name, std::string variable_2_name, 
        BoundaryName boundary)
{
    std::string block_name = std::string(boundary) + "_chemical_potential_interface";
    if (interface_exists_already(block_name))
    {
        return;
    }
    std::string type = "ADChemicalPotentialInterface";
    auto params = _factory.getValidParams(type);
    params.set<NonlinearVariableName>("variable") = variable_1_name;
    params.set<std::vector<VariableName>>("neighbor_var") = {variable_2_name};
    params.set<MaterialPropertyName>("s") = "S";
    params.set<MaterialPropertyName>("s_neighbour") = "S";
    // params.set<MaterialPropertyName>("D") = "D";
    // params.set<MaterialPropertyName>("D_neighbour") = "D";
    params.set<MaterialPropertyName>("rho") = "rho";
    params.set<MaterialPropertyName>("rho_neighbour") = "rho";
    params.set<std::vector<BoundaryName>>("boundary") = {boundary};

    // or just be lazy and shove this in a try/except block?
    try
    {
        _problem->addInterfaceKernel(type, block_name, params);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cout << exc.what();
        std::cout<<"Error intialising auxkernel for chempot continuity, block" << _blocks[0] << "\n";
    }
    catch(...) // what kind of error is thrown in the case of duplicates?
    {
        std::cout<<"Error intialising auxkernel for cehmpot continuity, block" << _blocks[0] << "\n";
    }
    // _problem->addAuxKernel(type, block_name, params);
    
}

void FosterMcNabbTrapAction::add_mass_continuity_interface(std::string variable_1_name, std::string variable_2_name, 
        BoundaryName boundary)
{
    std::string block_name = std::string(boundary) + "_mass_continuity_interface";
    if (interface_exists_already(block_name))
    {
        return;
    }
    std::string type = "ADMatInterfaceDiffusion";
    auto params = _factory.getValidParams(type);
    params.set<NonlinearVariableName>("variable") = variable_1_name;
    params.set<std::vector<VariableName>>("neighbor_var") = {variable_2_name};
    params.set<MaterialPropertyName>("D") = "D";
    params.set<MaterialPropertyName>("D_neighbour") = "D";
    params.set<MaterialPropertyName>("rho") = "rho";
    params.set<MaterialPropertyName>("rho_neighbour") = "rho";
    params.set<std::vector<BoundaryName>>("boundary") = {boundary};
    // _problem->addAuxKernel(type, block_name, params);
    try
    {
        _problem->addInterfaceKernel(type, block_name, params);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cout << exc.what();
        std::cout<<"Error intialising auxkernel for mass continuity, block" << _blocks[0] << "\n";
    }
    catch(...) // what kind of error is thrown in the case of duplicates?
    {
        std::cout<<"Error intialising auxkernel for mass continuity, block" << _blocks[0] << "\n";
    }
}

void FosterMcNabbTrapAction::add_mobile_concentration_interface(std::string variable_1_name, std::string variable_2_name, 
        BoundaryName boundary)
{
    std::string block_name = std::string(boundary) + "_mobile_concentration_interface";
    if (interface_exists_already(block_name))
    {
        return;
    }
    std::string type = "ADVariableMatch";
    auto params = _factory.getValidParams(type);
    params.set<NonlinearVariableName>("variable") = variable_1_name;
    params.set<std::vector<VariableName>>("neighbor_var") = {variable_2_name};
    params.set<MaterialPropertyName>("rho") = "rho";
    params.set<MaterialPropertyName>("rho_neighbour") = "rho";
    params.set<std::vector<BoundaryName>>("boundary") = {boundary};

    // std::string block_name = std::string(boundary) + "_mobile_concentration_interface";
    // _problem->addAuxKernel(type, block_name, params);
    try
    {
        _problem->addInterfaceKernel(type, block_name, params);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cout << exc.what();
        std::cout<<"Error intialising auxkernel for concentration continuity, block" << _blocks[0] << "\n";
    }
    catch(...) // what kind of error is thrown in the case of duplicates?
    {
        std::cout<<"Error intialising auxkernel for concentration continuity, block" << _blocks[0] << "\n";
    }
}