// FosterMcNabbTrapAction
#pragma once

// #include "TensorMechanicsActionBase.h"
include "Action.h"
#include "libmesh/point.h"

class FosterMcNabbTrapAction : public Action
{
public:
  static InputParameters validParams();

  FosterMcNabbTrapAction(const InputParameters & params);

  virtual void act();

private:
    struct arrhenius_parameters
    {
        Real v0;
        Real E;
    };
    std::map<std::string, FosterMcNabbTrapAction::trap_parameters> _traps;
    Real _k;
    const ADVariableValue & _temperature_variable;

protected:
    void addVariables(std::string names);
    void addMaterials();
    void addArrheniusMaterials(std::map<std::string, FosterMcNabbTrapAction::trap_parameters> & names);
    void addArrheniusMaterials(std::map<std::string, std::pair<Real, Real> & names);
    
    void addTrappingKernels();
    void addTrappingReactionKernels(std::vector<std::string>> trap_variables);
    void addTimeKernels(std::vector<std::string>> variables);
    void addTrapCouplingKernels(std::vector<std::pair<std::string,std::string>> mobile_trap_pairs);



  void actSubdomainChecks();
  void actOutputGeneration();
  void actEigenstrainNames();
  void actOutputMatProp();
  void actGatherActionParameters();
  void verifyOrderAndFamilyOutputs();
  void actLagrangianKernelStrain();
  void actStressDivergenceTensorsStrain();

  virtual std::string getKernelType();
  virtual InputParameters getKernelParameters(std::string type);

  /**
   * Helper function to decode `generate_outputs` options using a "table" of
   * scalar output quantities and a "setup" lambda that performs the input parameter
   * setup for the output material object.
   */
//   template <typename T, typename T2>
//   bool setupOutput(std::string out, T table, T2 setup);

//   ///@{ displacement variables
//   std::vector<VariableName> _displacements;

//   /// Number of displacement variables
//   unsigned int _ndisp;

//   /// Coupled displacement variables
//   std::vector<VariableName> _coupled_displacements;
//   ///@}

//   ///@{ residual debugging
//   std::vector<AuxVariableName> _save_in;
//   std::vector<AuxVariableName> _diag_save_in;
//   ///@}

//   Moose::CoordinateSystemType _coord_system;

//   /// if this vector is not empty the variables, kernels and materials are restricted to these subdomains
//   std::vector<SubdomainName> _subdomain_names;

//   /// set generated from the passed in vector of subdomain names
//   std::set<SubdomainID> _subdomain_ids;

//   /// set generated from the combined block restrictions of all TensorMechanics/Master action blocks
//   std::set<SubdomainID> _subdomain_id_union;

  const std::vector<MaterialPropertyName>> _material_definition_names;


  /// base name for the current master action block
  const std::string _base_name;

  /// use displaced mesh (true unless _strain is SMALL)
  bool _use_displaced_mesh;

  /// output materials to generate scalar stress/strain tensor quantities
  std::vector<std::string> _generate_output;
  MultiMooseEnum _material_output_order;
  MultiMooseEnum _material_output_family;


  /// automatically gather names of eigenstrain tensors provided by simulation objects
  const bool _auto_eigenstrain;

  std::vector<MaterialPropertyName> _eigenstrain_names;

  /// New or old kernel system
  const bool _lagrangian_kernels;

  const LKFormulation _lk_formulation;

  // Helper to translate into MOOSE talk
  static const std::map<unsigned int, std::string> _order_mapper;
  // Name of the homogenization scalar variable
  const std::string _hname = "hvar";
  // Name of the integrator
  const std::string _integrator_name = "integrator";
  // Name of the homogenization strain
  const std::string _homogenization_strain_name = "homogenization_gradient";
  // Other homogenization info
  MultiMooseEnum _constraint_types;
  std::vector<FunctionName> _targets;
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