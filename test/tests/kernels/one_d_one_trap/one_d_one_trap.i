[Mesh]
  type = GeneratedMesh
  dim = 1 
  nx = 20
  xmax = 5e-5
[]

[Problem]
  type = FEProblem # This is the "normal" type of Finite Element Problem in MOOSE
  coord_type = XYZ # cartesian
[]

[Variables]
  [./Mobile]
    initial_condition = 0.0
  [../]
  [./Trapped]
    initial_condition = 0.0
  [../]
[]

[Kernels]
  [./H3_diffusion_eq1]
    type = ADMatDiffusion
    variable = Mobile
    diffusivity = D
  [../]
  [./mobile_time_deriv]
    type = ADTimeDerivative
    variable = Mobile
  [../]
  [./trapping_equilibrium_equation]
    type = ADTrappingEquilibriumEquation
    variable = Trapped
    v = Mobile
    vi = V1
    n_traps = n1
  [../]
  [./trapped_time_deriv_couple]
    type = ADCoupledTimeDerivative 
    variable = Mobile
    v = Trapped
  [../]
  [./trapped_time_deriv]
    type = ADTimeDerivative
    variable = Trapped
  [../]
[]

[BCs]
  [./left_face]
    type = ADDirichletBC
    variable = Mobile
    boundary = left
    value = 1e-4
  [../]
  [./right_face]
    type = ADDirichletBC #DiffusionFluxBC
    variable = Mobile
    boundary = right
    value = 0
  [../]
[]

[Materials]
  [./mat1]
    type = TrappingMaterialConstT
    v0 = 1.0E13
    E1 = 8.6e-3
    E2 = 0.0
    E3 = 0.0
    k_boltz = 8.6E-5
    D0 = 1.0
    E_diff = 0.0
    lambda = 3.16E-8
    n_sol = 2
    n1 = 0.1
    n2 = 0.0
    n3 = 0.0
    const_T = 1000
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON 
  # Set PETSc parameters to optimize solver efficiency
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = ' hypre    boomeramg'
  dt = 1e-9
  num_steps = 10
[]

[Outputs]
  console = false
  exodus = true # Output Exodus format
[]

