[Mesh]
  type = GeneratedMesh
  dim = 1 
  nx = 50
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
  [./Trapped_1]
    initial_condition = 0.0
  [../]
  [./Trapped_2]
    initial_condition = 0.0
  [../]
  [./Trapped_3]
    initial_condition = 0.0
  [../]
[]

[AuxVariables]
  [Temperature]
    initial_condition = 1000
  []
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

  [./trapping_equilibrium_equation_1]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_1
    v = Mobile
    vi = V1
    n_traps = n1
  [../]
  [./trapped_time_deriv_couple_1]
    type = ADCoupledTimeDerivative 
    variable = Mobile
    v = Trapped_1
  [../]
  [./trapped_time_deriv_1]
    type = ADTimeDerivative
    variable = Trapped_1
  [../]

  [./trapping_equilibrium_equation_2]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_2
    v = Mobile
    vi = V2
    n_traps = n2
  [../]
  [./trapped_time_deriv_couple_2]
    type = ADCoupledTimeDerivative 
    variable = Mobile
    v = Trapped_2
  [../]
  [./trapped_time_deriv_2]
    type = ADTimeDerivative
    variable = Trapped_2
  [../]

  [./trapping_equilibrium_equation_3]
    type = ADTrappingEquilibriumEquation
    variable = Trapped_3
    v = Mobile
    vi = V3
    n_traps = n3
  [../]
  [./trapped_time_deriv_couple_3]
    type = ADCoupledTimeDerivative 
    variable = Mobile
    v = Trapped_2
  [../]
  [./trapped_time_deriv_3]
    type = ADTimeDerivative
    variable = Trapped_2
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
    type = TrappingMaterial
    n1 = 0.001
    n2  = 0.1
    n3 = 0.1
    v1 = 1e13
    v2 = 1e13
    v3 = 1e13
    E1 = 1e-2
    E2 = 8.6e-3
    E3 = 1e-4
    k_boltz = 8.6E-5
    D0 = 1.0
    E_diff = 0.0
    lambda = 3.16E-8
    n_sol = 2
    Temperature = Temperature
    Cp  =1
    conductivity = 1
    density = 1
  [../]
[]

[Postprocessors]
  [back_flux]
    type = ADSideFluxIntegral
    variable = Mobile
    boundary = right
    diffusivity = D
  [../]
[]

[Executioner]
  nl_abs_tol = 2e-12
  type = Transient
  solve_type = NEWTON 
  # Set PETSc parameters to optimize solver efficiency
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu  NONZERO'
  dt = 1e-9
  num_steps = 50
[]

[Outputs]
  console = false
  exodus = false # Output Exodus format
  csv = true
[]

