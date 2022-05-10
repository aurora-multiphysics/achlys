// DiffusiveMaterialAction
#pragma once

// #include "TensorMechanicsActionBase.h"
#include "MooseObject.h"
#include "Action.h"
#include "libmesh/point.h"
#include "BlockRestrictable.h"
#include "AddAuxVariableAction.h"
#include "NonlinearSystem.h"
#include "KernelBase.h"

#include<vector>
#include<string>

namespace AchlysConstants
{
    static const Real Boltzmann = 8.617e-5; // ev / K
    static const Real UiversalGas = 8.3145; // J / (mol K)
}

class DiffusiveMaterialAction : public Action
{
public:
  static InputParameters validParams();

  DiffusiveMaterialAction(const InputParameters & params);

  virtual void act();

  static MooseEnum VariableOrders();

protected:
    void addVariables();
    void addKernels();
    void addMaterials();

    void addArrheniusMaterial(std::string name, Real V0, Real E);
    void addGenericConstantMaterial(std::vector<std::string> names, std::vector<Real> values);
    void addParsedMaterial(std::string name, std::vector<std::string> args, std::string function);


    void addTimeKernels();
    void addDiffusionKernel();

    void addInterfaceKernels();
    bool interface_exists_already(std::string block_name);
    void add_chemical_potential_interface(std::string variable_1_name, std::string variable_2_name, 
        BoundaryName boundary);
    void add_mass_continuity_interface(std::string variable_1_name, std::string variable_2_name, 
        BoundaryName boundary);
    void add_mobile_concentration_interface(std::string variable_1_name, std::string variable_2_name, 
        BoundaryName boundary);
    void add_chemical_potential_based_interface();
    void add_concentration_based_interface();
    SubdomainName get_neighbour_block_name(std::string boundary_name);

    void addAuxVariables();
    void addAuxKernels();

    void add_parsed_aux(std::string name, std::vector<std::string> args, std::string function);
    // void add_parsed_aux(std::string name, std::vector<std::string> args, std::vector<std::string> const_vars,
                        //  std::vector<std::string> const_vals, std::string function);
    void add_continuous_mobile_aux();
    // void add_total_trapped_aux();
    void add_total_retention_aux();

    void add_aux_variable(std::string name, bool second_order);
    void add_aux_variable(std::string name);
    // void add_parsed_aux(std::string name, std::vector<std::string> args, std::string function);

    // std::vector<Real> _n;
    // std::vector<Real> _v0;
    // std::vector<Real> _E;
    // const ADVariableValue & _temperature_variable;
    std::string _temperature_variable;


    Real _k;
    Real _D0;
    Real _Ed;
    Real _S0;
    Real _Es;
    Real _rho;

    std::string _mobile_variable_base;
    

    std::string _diffusivity_material_base;
    std::string _solubility_material_base;

    std::vector<SubdomainName> _blocks;
    std::vector<std::string> _requested_aux_variables;

    // bool _transient;

    int _n_traps;

    std::string _mobile_variable_name;
    std::vector<std::string> _all_variable_names;
    std::string _diffusivity_material_name;
    std::string _solubility_material_name;

    std::string _block_prepend;

    std::vector<std::string> _aux_variable_names;
    std::vector<std::string> _solid_boundaries;

    bool _solubility_specified;

    // MooseEnum _interface_type;
    enum class InterfaceType
    {
        chemical_potential,
        concentration
    };
    InterfaceType _interface_type;

    bool _variable_order_specified;
    MooseEnum _variable_order;

    enum class EnergyUnits
    {
        eV,
        kJ
    };
    EnergyUnits _energy_units;

    enum class ConcentrationUnits
    {
        atomic_fraction,
        moles_per_m3,
        atoms_per_m3
    };
    ConcentrationUnits _concentration_units;

};

MooseEnum
DiffusiveMaterialAction::VariableOrders()
{
  auto orders = AddAuxVariableAction::getAuxVariableOrders().getRawNames();

  return MooseEnum(orders, "FIRST");
}