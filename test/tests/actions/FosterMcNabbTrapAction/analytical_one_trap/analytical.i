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
  [./mobile]
    initial_condition = 0.0
  [../]
  [./trap_1]
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
    variable = mobile
    diffusivity = D
  [../]
  [./mobile_time_deriv]
    type = ADTimeDerivative
    variable = mobile
  [../]
  [./trapping_equilibrium_equation]
    type = ADTrappingEquilibriumEquation
    variable = trap_1
    v = mobile
    vi = V1
    n_traps = n1
  [../]
  [./trapped_time_deriv_couple]
    type = ADCoupledTimeDerivative 
    variable = mobile
    v = trap_1
  [../]
  [./trapped_time_deriv]
    type = ADTimeDerivative
    variable = trap_1
  [../]
[]

[BCs]
  [./left_face]
    type = ADDirichletBC
    variable = mobile
    boundary = left
    value = 1e-4
  [../]
  [./right_face]
    type = ADDirichletBC
    variable = mobile
    boundary = right
    value = 0
  [../]
[]

[Materials]
  [./mat1]
    type = TrappingMaterial
    v1 = 1.0E13
    v2 = 1.0E13
    v3 = 1.0E13
    E1 = 8.6e-3
    E2 = 0.0
    E3 = 0.0
    k_boltz = 8.617E-5
    D0 = 1.0
    E_diff = 0.0
    lambda = 3.16E-8
    n_sol = 2
    n1 = 0.1
    n2 = 0.0
    n3 = 0.0
    Temperature = Temperature
    Cp=0
    conductivity=0
    density=0
  [../]
[]

[Postprocessors]
  [back_flux]
    type = ADSideDiffusiveFluxIntegral
    variable = mobile
    boundary = right
    diffusivity = D
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON 
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu  NONZERO'
  dt = 1e-10
  num_steps = 100
[]

[Outputs]
  console = false
  exodus = true # Output Exodus format
  csv = true
[]

