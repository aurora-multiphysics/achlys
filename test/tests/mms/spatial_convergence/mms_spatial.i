[Mesh]
  type = GeneratedMesh # Can generate simple lines, rectangles and rectangular prisms
  dim = 1 # Dimension of the mesh
  nx = 4 # Number of elements in the x direction
  xmax = 1.0 # Length of test chamber
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
#    scaling = 1e18
  [../]
[]

#[ICs]
#  [./Mobile]
#    type = FunctionIC
#    function = '1 + ( x * x)'
#    variable = Mobile
#  [../]
#  [./Trapped]
#    type = FunctionIC
#    function = '2 + ( x * x)'
#    variable = Trapped
#  [../]
#[]

[Functions]
  [spatial_convergence_f]
    type = ParsedFunction
    value = '(1/tau) * sin(2*pi*x/l) + D*((2*pi/l)^2) * sin(2*pi*x/l)*(t/tau) + (1/tau)*(x/l)^3'
    vars = 'l D tau'
    vals = '1.0 1.0 1.0' #4.4e-9
  []
  [spatial_convergence_g]
    type = ParsedFunction
    value = '(1/tau) * (x/l)^3 - (vm * sin(2*pi*x/l) * t * (n - (t*(x/l)^3))) + (vi*(t*(x/l)^3))'
    vars = 'l D vm vi n tau'
    vals = '1.0 1.0 0.5 1.0 2.0 1.0'
  []
  [exact]
    type = ParsedFunction
    value = 'sin(2*pi*x/l) * (t/tau)'
    vars = 'l tau'
    vals = '1.0 1.0'
  []
  #[temporal_convergence_f]
  #  type = ParsedFunction
  #  value = ''
  #  vars = 'l D'
  #  vals = '0.02 4.4e-9'
  #[]
  #[temporal_convergence_g]
  #  type = ParsedFunction
  #  value = ''
  #  vars = 'l D'
  #  vals = '0.02 4.4e-9'
  #[]
[]

[Postprocessors]
[error]
    type = ElementL2Error
    function = exact
    variable = Mobile
  []
  [h]
    type = AverageElementSize
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
# EXTRA MMS SOURCE TERMS
 [./eq1_mms_source]
   variable = Mobile
   type = ADBodyForce
   function = spatial_convergence_f
   value = 1
 [../]
 [./eq2_mms_source]
   variable = Trapped
   type = ADBodyForce
   function = spatial_convergence_g
   value = 1
 [../]
[]

[BCs]
  [./left_face]
    type = ADDirichletBC
    variable = Mobile
    boundary = left
    value = 0
  [../]
  [./right_face]
    type = ADDirichletBC
    variable = Mobile
    boundary = right
    value = 0
  [../]
  [./left_face_t]
    type = ADDirichletBC
    variable = Trapped
    boundary = left
    value = 0
  [../]
  [./right_face_t]
    type = ADFunctionDirichletBC
    variable = Trapped
    boundary = right
    function = 't'
  [../]
[]
[Materials]
  [./mat1]
    type = TrappingMaterialConstT
    v0 = 1.0
    E1 = 0.0
    E2 = 0.0
    E3 = 0.0
    k_boltz = 8.6E-5
    D0 = 1.0 #2.1019185 #4.1e-7
    E_diff = 0.0
    lambda = 0.4472135954999579
    n_sol = 10
    n1 = 2
    n2 = 0.0
    n3 = 0.0
    const_T = 1000
  [../]
[]
[Executioner]
  type = Transient # Steady state problem
  solve_type = NEWTON #'PJFNK' #NEWTON # Perform a Newton solve
  compute_scaling_once = False
  automatic_scaling = True
  #  l_tol = 1e-4
 #   nl_rel_tol = 1e-4
  #  nl_abs_tol = 5e-30
#  resid_vs_jac_scaling_param = 1.0
  nl_max_funcs = 7
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  dt = 1e-2
  end_time = 1.0
  timestep_tolerance = 1e-5
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
   #[./prov]
  #  type = Provenance
  #[../]
[]
#[Debug]
#  show_material_props = true
#  show_var_residual_norms = true
#[]
