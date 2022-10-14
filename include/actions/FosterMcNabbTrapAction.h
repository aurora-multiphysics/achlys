// FosterMcNabbTrapAction
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

#include "DiffusiveMaterialAction.h"
// #include "nlohmann/json.h"
// #include <map>

// template<class T>
// using VariableMap = std::vector<std::map<std::string, T>>;
// using VariableMap = std::map<std::string, std::pair<std::string, std::string>>;
// using json = nlohmann::json;

// class VariableMaps
// {
//     VariableMap<bool> bools;
//     VariableMap<int> ints;
//     VariableMap<Real> reals;
//     VariableMap<std::string> strings;
//     VariableMap<std::vector<Real>> real_vectors;
//     VariableMap<std::vector<std::string>> string_vectors;
// };

class FosterMcNabbTrapAction : public DiffusiveMaterialAction
{
public:
  static InputParameters validParams();

  FosterMcNabbTrapAction(const InputParameters & params);

  virtual void act();

protected:
    // void addVariables();
    void addKernels();
    void addMaterials();

    void addParsedMaterial(std::string name, std::vector<std::string> args, std::string function);

    void addTrappingKernels();
    void addTrappingReactionKernels();
    void addDetrappingRateMaterials();
    void addTimeKernels();
    void addTrapCouplingKernels();
    void addAuxKernels();

    void add_total_trapped_aux();
    void add_total_retention_aux();

    std::vector<Real> _n;
    std::vector<Real> _v0;
    std::vector<Real> _E;

    Real _p0;
    Real _Ep;
    Real _lambda;
    Real _n_sol;
    Real _rho;

    std::string _trap_variable_base;
    std::string _detrap_material_base;
    
    std::string _trap_material_base;
    std::string _trap_density_material_base;

    int _n_traps;

    std::string _trapping_rate_material_name;
    
    std::vector<std::string> _trap_variable_names;
    std::vector<std::string> _trap_density_names;
    std::vector<std::string> _detrapping_rate_names;

    bool _trapping_rate_specified;
    bool _trapping_energy_specified;

    public:
    // used for unit testing
    inline virtual void serialise_to_json(json & j) const
    {
        DiffusiveMaterialAction::serialise_to_json(j);
        json & k = _blocks.empty() ? j : j[_blocks[0]];
        k["n"] = _n;
        k["v0"] = _v0;
        k["E"] = _E;
        k["p0"] = _p0;
        k["Ep"] = _Ep;
        k["lambda"] = _lambda;
        k["n_sol"] = _n_sol;
        // k["rho"] = _rho;
        k["n_traps"] = _n_traps;
        k["trap_variable_base"] = _trap_variable_base;
        k["detrap_material_base"] =  _detrap_material_base;
        k["trap_material_base"] = _trap_material_base;
        k["trap_density_material_base"] = _trap_density_material_base;
        k["trapping_rate_material_name"] = _trapping_rate_material_name;
        k["trap_variable_names"] = _trap_variable_names;
        k["trap_density_names"] = _trap_density_names;
        k["detrapping_rate_names"] = _detrapping_rate_names;
        k["trapping_rate_specified"] = _trapping_rate_specified;
        k["trapping_energy_specified"] = _trapping_energy_specified;
    }
};

void to_json(json & j, FosterMcNabbTrapAction trap_action)
{
    trap_action.serialise_to_json(j);
}