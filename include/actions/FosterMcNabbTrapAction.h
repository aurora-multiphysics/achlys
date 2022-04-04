// FosterMcNabbTrapAction
#pragma once

// #include "TensorMechanicsActionBase.h"
#include "MooseObject.h"
#include "Action.h"
#include "libmesh/point.h"
#include "BlockRestrictable.h"
#include "AddAuxVariableAction.h"

#include<vector>
#include<string>

class FosterMcNabbTrapAction : public Action
{
public:
  static InputParameters validParams();

  FosterMcNabbTrapAction(const InputParameters & params);

  virtual void act();

protected:
    void addVariables();
    void addMaterials();
    // svoid addArrheniusMaterials(std::map<std::string, FosterMcNabbTrapAction::trap_parameters> & names);
    // void addArrheniusMaterials(std::map<std::string, std::pair<Real, Real> & names);
    void addArrheniusMaterial(std::string name, Real V0, Real E);
    void addGenericConstantMaterial(std::vector<std::string> names, std::vector<Real> values);

    void addTrappingKernels();
    void addTrappingReactionKernels();
    void addDetrappingRateMaterials();
    void addTimeKernels();
    void addTrapCouplingKernels();
    void addDiffusionKernel();

    void addAuxVariables();
    void addAuxKernels();

    void add_aux_variable(std::string name, bool second_order);
    void add_parsed_aux(std::string name, std::vector<std::string> args, std::string function);

    std::vector<Real> _n;
    std::vector<Real> _v0;
    std::vector<Real> _E;
    // const ADVariableValue & _temperature_variable;
    std::string _temperature_variable;


    Real _k;
    Real _D0;
    Real _Ed;
    Real _S0;
    Real _Es;
    Real _p0;
    Real _Ep;
    Real _lambda;
    Real _n_sol;
    Real _rho;

    std::string _trap_variable_base;
    std::string _mobile_variable_base;
    std::string _detrap_material_base;
    
    std::string _trap_material_base;
    std::string _trap_density_material_base;
    std::string _diffusivity_material_base;
    std::string _solubility_material_base;

    std::vector<SubdomainName> _blocks;

    // bool _transient;

    int _n_traps;

    std::string _mobile_variable_name;
    std::string _trapping_rate_material_name;
    std::string _diffusivity_material_name;
    std::string _solubility_material_name;

    std::string _block_prepend;
    
    std::vector<std::string> _trap_variable_names;
    std::vector<std::string> _all_variable_names;
    std::vector<std::string> _trap_density_names;
    std::vector<std::string> _detrapping_rate_names;

    // option to use specified value for the trapping reaction rate
    bool _trapping_rate_specified;
    bool _trapping_energy_specified;
    bool _solubility_specified;

    bool _variable_order_specified;
    MultiMooseEnum _variable_order;
    MultiMooseEnum VariableOrders();

    MultiMooseEnum _interface_type;
    enum class InterfaceType
    {
        chemical_potential,
        concentration
    }


};

MultiMooseEnum
FosterMcNabbTrapAction::VariableOrders()
{
  auto orders = AddAuxVariableAction::getAuxVariableOrders().getRawNames();

  return MultiMooseEnum(orders);
}