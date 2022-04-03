// FosterMcNabbTrapAction
#pragma once

// #include "TensorMechanicsActionBase.h"
#include "MooseObject.h"
#include "Action.h"
#include "libmesh/point.h"
#include "BlockRestrictable.h"

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

    std::vector<SubdomainName> _blocks;

    int _n_traps;

    std::string _mobile_variable_name;
    std::string _trapping_rate_material_name;

    std::string _block_prepend;
    
    std::vector<std::string> _trap_variable_names;
    std::vector<std::string> _all_variable_names;
    std::vector<std::string> _trap_density_names;
    std::vector<std::string> _detrapping_rate_names;

    // option to use specified value for the trapping reaction rate
    bool _trapping_rate_specified;
    bool _trapping_energy_specified;
    bool _solubility_specified;


//   void actSubdomainChecks();
//   void actOutputGeneration();
//   void actEigenstrainNames();
//   void actOutputMatProp();
//   void actGatherActionParameters();
//   void verifyOrderAndFamilyOutputs();
//   void actLagrangianKernelStrain();
//   void actStressDivergenceTensorsStrain();

//   virtual std::string getKernelType();
//   virtual InputParameters getKernelParameters(std::string type);


  
//   struct arrhenius_parameters
//     {
//         arrhenius_parameters(Real v0, Real E):v0(v0),E(E){};
//         Real v0;
//         Real E;
//     };

//     std::map<std::string, FosterMcNabbTrapAction::arrhenius_parameters> _traps;
//     const ADVariableValue & _temperature_variable;
//     const std::vector<MaterialPropertyName>> _material_definition_names;

};

// template <typename T, typename T2>
// bool
// TensorMechanicsAction::setupOutput(std::string out, T table, T2 setup)
// {
//   for (const auto & t1 : table)
//   {
//     // find the officially supported properties
//     for (const auto & t2 : t1.second.second)
//       if (t1.first + '_' + t2 == out)
//       {
//         const auto it = _rank_two_cartesian_component_table.find(t2);
//         if (it != _rank_two_cartesian_component_table.end())
//         {
//           setup(it->second, t1.second.first);
//           return true;
//         }
//         else
//           mooseError("Internal error. The permitted tensor shortcuts must be keys in the "
//                      "'_rank_two_cartesian_component_table'.");
//       }

//     // check for custom properties
//     auto prefix = t1.first + '_';
//     if (out.substr(0, prefix.length()) == prefix)
//     {
//       setup(out.substr(prefix.length()), t1.second.first);
//       return true;
//     }
//   }

//   return false;
// }