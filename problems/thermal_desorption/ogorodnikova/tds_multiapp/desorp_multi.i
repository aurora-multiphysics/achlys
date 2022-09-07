[Mesh]
 [./unlabelled]
    file = ogorodnikova_1k.msh
    type = FileMeshGenerator
    construct_side_list_from_node_list=true
  [../]
  [./block_1]
    type= AllSideSetsByNormalsGenerator
    input = unlabelled
    #fixed_normal = false
  [../]
[]

[Problem]
  type = FEProblem # This is the "normal" type of Finite Element Problem in MOOSE
  coord_type = XYZ # cartesian
[]

[Variables]
  [./Mobile]
  [../]
  [./Trapped_1]
  [../]
  [./Trapped_2]
  []
  [./Trapped_3]
  []
[]

[Functions]
  [./dts]
    type = PiecewiseLinear
    x = '0 1e-4 5e-4 1e-3'
    y = '1e-4 1e-4 5e-4 5e-4'
  [../]
  [./Gaussian_implant]
    type = ParsedFunction
    value = 'scale * exp( -0.5 * ((x - mean) / sd)^2)'
    vars = 'scale mean sd'
    vals = '0.93e8 4.5e-9 4.5e-9'
  [../]
  [./Exponweib_implant]
    type = ParsedFunction
    value = 'peak * (a * c / scale) * (( 1 - exp(-1 * (((x*1e9)-loc)/scale)^c))^(a-1) ) * exp(-1 * (((x*1e9)-loc)/scale)^c)*(((x*1e9)-loc)/scale)^(c-1)'
    vars = 'peak a c loc scale'
    vals = '5.82e8 0.90 1.96 -0.013 4.909'
  [../]
[]

[MultiApps]
  [sub_app]
    type = FullSolveMultiApp
    #app_type = BlueKiteApp
    input_files = 'resting_multi.i'
   # positions = '0   0.029   0
   #              0.015 0.0.029 0
   #              0.015 0.014 0
   #              0.0 0.014 0'
  execute_on = INITIAL
  clone_master_mesh = true
  []
[]

[Transfers]
#   [to_sub] 
#    type = MultiAppCopyTransfer
#    multi_app = sub_app
#    source_variable = 'surface_temp'
#    variable = 'surface_temp'
#    direction = to_multiapp
#  []
  [mobile_transfer]
    type =  MultiAppCopyTransfer #MultiAppScalarToAuxScalarTransfer
    multi_app = sub_app
    source_variable = 'Mobile'
    variable = 'Mobile'
    direction = from_multiapp
  []
  [trap_1_transfer]
    type =  MultiAppCopyTransfer #MultiAppScalarToAuxScalarTransfer
    multi_app = sub_app
    source_variable = 'Trapped_1'
    variable = 'Trapped_1'
    direction = from_multiapp
  []
  [trap_2_transfer]
    type =  MultiAppCopyTransfer #MultiAppScalarToAuxScalarTransfer
    multi_app = sub_app
    source_variable = 'Trapped_2'
    variable = 'Trapped_2'
    direction = from_multiapp
  []
  [trap_3_transfer]
    type =  MultiAppCopyTransfer #MultiAppScalarToAuxScalarTransfer
    multi_app = sub_app
    source_variable = 'Trapped_3'
    variable = 'Trapped_3'
    direction = from_multiapp
  []
[]

[Kernels]
  [./H3_diffusion_eq1]
    type = ADMatDiffusion
    variable = Mobile
    Diffusivity = D
  [../]
  [./mobile_time_deriv]
    type = ADTimeDerivative
    variable = Mobile
  [../]
  [./trapping_equilibrium_equation1]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_1
    v = Mobile
    n_traps = n1
    vi = V1
  [../]
  [./trapping_equilibrium_equation2]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_2
    v = Mobile
    n_traps = n2
    vi = V2
  [../]
  [./trapping_equilibrium_equation3]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_3
    v = Mobile
    n_traps = n3
    vi = V3
  [../]
  [./trapped_time_deriv_couple]
    type = ADCoupledTimeDerivative
    variable = Mobile
    v = Trapped_1
  [../]
  [./trapped_time_deriv_couple2]
    type = ADCoupledTimeDerivative
    variable = Mobile
    v = Trapped_2
  [../]
  [./trapped_time_deriv_couple3]
    type = ADCoupledTimeDerivative
    variable = Mobile
    v = Trapped_3
  [../]
  [./trapped_time_deriv]
    type = ADTimeDerivative
    variable = Trapped_1
  [../]     
  [./trapped_time_deriv2]
    type = ADTimeDerivative
    variable = Trapped_2
  [../]
  [./trapped_time_deriv3]
    type = ADTimeDerivative
    variable = Trapped_3
  [../]
[]

[Postprocessors]
  [/pfc_flux]
    type = ADSideFluxIntegral
    variable = Mobile
    boundary = 1
    diffusivity = D
  [../]
  [/back_flux]
    type = ADSideFluxIntegral
    variable = Mobile
    boundary = 2
    diffusivity = D
  [../]
  [./total_mobile]
    type = VariableIntegral
    variable = Mobile
  [../]
  [./total_trap_1]
    type = VariableIntegral
    variable = Trapped_1
  [../]
    [./total_trap_2]
    type = VariableIntegral
    variable = Trapped_2
  [../]
  [./total_trap_3]
    type = VariableIntegral
    variable = Trapped_3
  [../]
[]

[BCs]
  [./desorption]
    type =  ADDirichletBC
    variable = Mobile
    boundary = 1
    value = 0
  [../]
  [./back_desorption]
    type =  ADDirichletBC
    variable = Mobile
    boundary = 2
    value = 0
  [../]
[]

[Materials]
   [./implant]
    type = ExtrinsicStaticTrappingMaterialRampingT
# Energies
    E_diff = 0.39
    E1 = 0.87
    E2 = 1.0
    E3 = 1.50
    k_boltz = 8.617333E-5
# pre-exponential rate constants
    v0 = 1.0E13
    D0 = 4.1E-7
    lambda = 1.1E-10
# unused
    rho = 1 # unecessary scaling factor, do not use
# site densities
    n_sol = 6
    n1 = 1E-3 #1.0E25
    n2 = 4e-4
    n3a_max = 1e-1
    n3b_max = 1e-2
# trap creation rates
    eta_a = 6e-4
    eta_b = 2e-4
    trap_evolution_time = 400
# flux distribution parameters
    flux = 4e-10
    function = Gaussian_implant
    xp = 1e-6
#Temperature
    initial_T = 300
    beta = 8
    block = 'Tungsten' # 'Stopping_zone' # Plastic_region
# thermal properties
    conductivity = 150 # W/K
    Cp = 137 # J/(kg K)
    density = 19300 # kg/m3
  [../] 
[]
[Executioner]
  type = Transient
  solve_type = NEWTON
  scheme = bdf2
  automatic_scaling=True
 compute_scaling_once=False
  scaling_group_variables = 'Trapped_3; Trapped_2; Trapped_1; Mobile'
  resid_vs_jac_scaling_param = 0.8
 # l_tol = 1e-4
  l_max_its = 100
  nl_max_funcs = 7
  #nl_rel_tol = 1e-7
  nl_abs_tol = 1e-29

  # Set PETSc parameters to optimize solver efficiency
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu  NONZERO'
#  dt = 0.25 #0.001 #5e-2
  end_time = 62.5
  timestep_tolerance = 0.01
    [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 5
    cutback_factor = 0.5
    growth_factor = 1.1
    dt = 0.1
  []
[]


[Outputs]
  #execute_on = 'timestep_end'
  exodus = false # Output Exodus format
  csv = true
[]
[Debug]
  show_material_props = true
  show_var_residual_norms = true
[]
