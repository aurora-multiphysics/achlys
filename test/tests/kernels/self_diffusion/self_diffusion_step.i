[Mesh]
  [bulk_mesh]
    type = GeneratedMeshGenerator   
    dim=1
    xmin=0
    xmax=10
    nx=200
  []
[]

[Variables]
  [H_Mobile]
    [InitialCondition]
      type = FunctionIC
     function = left
    []
#    initial_condition = 2e-12
  []

  [D_Mobile]
    [InitialCondition]
      type = FunctionIC
      function = right
    []
#    initial_condition = 0.0 
 []
[]

[AuxVariables]
  [Temperature]
   initial_condition = 1000
    family = MONOMIAL
    order = CONSTANT
  []
[]

[Functions]
  [gaussian]
    type = ParsedFunction
    vars = 'sd mu'
    vals = '1e-6 10e-6'
    value = '1/ (sd * sqrt(2*pi)) * exp(-0.5 * (x-mu)^2 / sd^2)'
  []
#  [implant]
#    type = ParsedFunction
#    vars = 'gaussian'
#    vals = 'gaussian'
#    value = if(t<500, gaussian, 0)
#  []
  [left]
    type=ParsedFunction
    vars = 'low high L'
    vals = '0 1e-14 1e-3'
    value = 'if(x<L/2, high, if(x=L/2, high/2, low))'
  []
  [right]
    type=ParsedFunction
    vars = 'low high L'
    vals = '0 1e-14 1e-3'
    value = 'if(x>L/2, high, if(x=L/2, high/2, low))'
  []
  [edges]
    type=ParsedFunction
    vars = 'low high L'
    vals = '0 1e-14 1e-3'
    value = 'if(x<L/2, high, if(x=L/2, high/2, low))'
  []
[]


[Kernels]
  [H_simple_diffusion]
    type = ADMatDiffusion
    variable = H_Mobile
    diffusivity = D1
  []
  [H_coupled_diffusion]
    type = ADCoupledIsotopeDiffusion
    variable = H_Mobile
    v = D_Mobile
    D1 = D1
    D2 = D2
  []
  [H_mobile_time_deriv]
    type = ADTimeDerivative
    variable = H_Mobile
[]



  #[D_source]
  #  type = ADBodyForce
  #  variable = D_Mobile
  #  value = 4e-15
  #  function = gaussian # implant
  #[]
  [D_simple_diffusion]
    type = ADMatDiffusion
    variable = D_Mobile
    diffusivity = D2
  []
  [D_coupled_diffusion]
    type = ADCoupledIsotopeDiffusion
    variable = D_Mobile
    v = H_Mobile
    D1 = D2
    D2 = D1
  []
  [D_mobile_time_deriv]
    type = ADTimeDerivative
    variable = D_Mobile
  []

[]

[Materials]
   [bulk]
    type = BinaryTrappingMaterial
# Energies
    E_diff_1 = 0.39
    E_diff_2 = 0.39
    E1 = 0.87
    E2 = 1.0
    E3 = 0.0
    k_boltz = 8.617333E-5
# pre-exponential rate constants
    v1 = 1e13
    v2 = 1e13
    v3 = 1e13
    D0_1 = 4.1e-3
    D0_2 = 2.9e-3
# m_H = 1.6735575e-27
# m_D = 3.3435e-27
    lambda = 1.1E-10
# site densities
    n_sol = 6
    n1 = 1e-3
    n2 = 4e-4
    n3 = 0
#Temperature
    Temperature = Temperature
# thermal properties
    conductivity = 150 # W/K
    Cp = 137 # J/(kg K)
    density = 19300 # kg/m3
  [../]


  [Total_HI]
    type=ParsedMaterial
    f_name = total_HI
    args = 'H_Mobile D_Mobile'
    function = 'H_Mobile + D_Mobile'
    outputs = exodus
  []
 # [Total_trap_1]
 #   type=ParsedMaterial
 #   f_name = total_trap_1
 #   args = 'H_Trapped_1 D_Trapped_1'
 #   function = 'H_Trapped_1 + D_Trapped_1'
 #   outputs = exodus
 # []
 # [Total_trap_2]
 #   type=ParsedMaterial
 #   f_name = total_trap_2
 ##   args = 'H_Trapped_2 D_Trapped_2'
 #   function = 'H_Trapped_2 + D_Trapped_2'
 #   outputs = exodus
 # []
[]

#[BCs]
#  [H0]
#    type = ADDirichletBC
#    variable = H_Mobile
#    boundary = '0 1'
#    value = 0
#  [../]
#  [D0]
#    type = ADDirichletBC
#    variable = D_Mobile
#    boundary = '0 1'
#    value = 0
#  [../]
#[]


[Executioner]
  scheme = bdf2
  type = Transient
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -ksp_type -pc_factor_shift_type'
  petsc_options_value = 'lu bcgs NONZERO'
  end_time = 10
#  [TimeStepper]
#    type = IterationAdaptiveDT
#    optimal_iterations = 6
#    cutback_factor = 0.8
#    growth_factor = 1.2
#    dt = 5e-8
#  []
  dt = 10  # seconds
  timestep_tolerance = 1e-8 # seconds
  automatic_scaling = True
  compute_scaling_once = False
  #nl_rel_tol = 1e-4
  #nl_abs_tol = 1e-25 # This should avoid solve failures when close to steady-state
  nl_max_its = 10
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  exodus = true
  console = true
[]

[Debug]
  show_var_residual_norms = true
[]
