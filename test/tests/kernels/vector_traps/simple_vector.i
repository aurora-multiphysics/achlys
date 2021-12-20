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
    type = ADVectorTrappingEquilibriumEquation
    variable = Trapped_1
    v = Mobile
    vi = Vi
    n_traps = ni
    index = 1
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
    type = ADVectorTrappingEquilibriumEquation
    variable = Trapped_2
    v = Mobile
    vi = Vi
    n_traps = ni
    index = 2
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
    type = ADVectorTrappingEquilibriumEquation
    variable = Trapped_3
    v = Mobile
    vi = Vi
    n_traps = ni
    index = 3
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
    type = VectorTrappingMaterial
    ni = '0.001 0.1 0.1'
    v0 = '1.0E13 1.0E13 1.0E13'
    Ei = '1e-2 8.6e-3 1e-4'
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

[Executioner]
  nl_abs_tol = 2e-12
  nl_rel_tol = 1e-5
  type = Transient
  solve_type = NEWTON 
  # Set PETSc parameters to optimize solver efficiency
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu  NONZERO'
  dt = 1e-9
  num_steps = 20
[]

[Postprocessors]
  [back_flux]
    type = ADSideFluxIntegral
    variable = Mobile
    boundary = right
    diffusivity = D
  [../]
[]

[Outputs]
  console = false
  exodus = false # Output Exodus format
  csv = true
[]

