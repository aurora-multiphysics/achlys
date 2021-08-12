[Mesh]
  [./unlabelled]
    file = monoblock.msh
    type = FileMeshGenerator
    construct_side_list_from_node_list=true
  [../] 
  [./block_1]
    type= AllSideSetsByNormalsGenerator
    input = unlabelled
    fixed_normal = false
  [../]
[]

[Problem]
  type = FEProblem # This is the "normal" type of Finite Element Problem in MOOSE
  coord_type = XYZ # cartesian
[]

[Variables]
  [./Temperature]
    initial_condition = 323
  [../]
[]

[Kernels]
  [./Temperature_evolution]
    variable = Temperature
    type = ADHeatConduction
    thermal_conductivity = conductivity
  [../]
[]
[BCs]
  [./pfc_t]
    type = ADNeumannBC 
    variable = Temperature
    boundary = 2
    value = 1.00e+7 # W/m2
  [../]
  [./cooling_t]
    type = ADConvectiveHeatFluxBC
    variable = Temperature
    boundary = 5
    T_infinity = 323
    heat_transfer_coefficient = 70000
  [../]
[]

[Materials]
   [./W]
    type = TrappingMaterial
    v1 = 1.0E13
    v2 = 1.0E13
    v3 = 1.0E13
    E1 = 0.86
    E2 = 1.0
    E3 = 0.0
    k_boltz = 8.6E-5
    D0 = 4.1E-7
    E_diff = 0.39
    lambda = 1.1E-10
    n_sol = 6
    n1 = 1E-3 #1.0E25
    n2 = 4e-4
    n3 = 0.0
    Temperature = Temperature
    block = 'W'
# thermal properties
    conductivity = 150 # W/K
    Cp = 137 # J/(kg K)
    density = 19300 # kg/m3
  [../]
  [./Cu]
    type = TrappingMaterial
    v1 = 1.0E13
    v2 = 1.0E13
    v3 = 1.0E13
    E1 = 0.0
    E2 = 0.0
    E3 = 0.0
    k_boltz = 8.6E-5
    D0 = 3.5E-5
    E_diff = 0.67
    lambda = 1.1E-10
    n_sol = 6
    n1 = 0.0 #1.0E25
    n2 = 0.0
    n3 = 0.0
    Temperature = Temperature
    block = 'Cu'
# thermal properties
    conductivity = 350 # W/K
    Cp = 380 # J/(kg K)
    density = 8900 # kg/m3
  [../]
  [./CuCrZr]
    type = TrappingMaterial
    v1 = 1.0E13
    v2 = 1.0E13
    v3 = 1.0E13
    E1 = 0.0
    E2 = 0.0
    E3 = 0.5
    k_boltz = 8.6E-5
    D0 = 6.6E-7
    E_diff = 1.00
    lambda = 1.1E-10
    n_sol = 6
    n1 = 0.0
    n2 = 0.0
    n3 = 5.87e-5
    Temperature = Temperature
    block = 'CuCrZr'
# thermal properties
    conductivity = 300  # W/K
    Cp = 400 # J/(kg K)
    density = 8960 # kg/m3
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  automatic_scaling = true
  # compute_scaling_once = false
  # l_tol = 1e-10
  # nl_rel_tol = 1e-7
  #resid_vs_jac_scaling_param = 0.5
  petsc_options_iname = '-ksp_type -pc_type -snes_type'
  petsc_options_value = 'bcgs lu fas'
[]
[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]
[Outputs]
  exodus = true # Output Exodus format
  csv = true
[]
[Debug]
  show_material_props = true
  show_var_residual_norms = true
[]

[Postprocessors]
  [/pfc_flux]
    type = ADSideFluxIntegral
    variable = Temperature
    boundary = 2
    diffusivity = conductivity #thermal_diffusivity #conductivity
  [../]
  [cooling_surface_flux]
    type = ADSideFluxIntegral
    variable = Temperature
    boundary = 5
    diffusivity = conductivity
  [../]
  [./max_ab]
    type = SideMaxValue
    v = Temperature
    start_point = '0.015 0.029 0'
    end_point = '0.015 0.0205 0'
  [../]
  [./min_ab]
    type = SideMinValue
    v = Temperature
    start_point = '0.015 0.029 0'
    end_point = '0.015 0.0205 0'
  [../]
[]
