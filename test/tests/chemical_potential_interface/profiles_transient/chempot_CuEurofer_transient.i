[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 3000
    xmax = 4e-3
    elem_type = EDGE3
  []

  # assign two subdomains
  [block1]
    type = SubdomainBoundingBoxGenerator
    input = generated
    block_id = 1
    bottom_left = '0 0 0'
    top_right = '2e-3 0 0'
  []
  [block2]
    type = SubdomainBoundingBoxGenerator
    input = block1
    block_id = 2
    bottom_left = '2e-3 0 0'
    top_right = '4e-3 0 0'
  []
  [interface]
    type = SideSetsBetweenSubdomainsGenerator
    input = block2
    primary_block = '1'
    paired_block = '2'
    new_boundary = 'interface'
  []
  [break_boundary]
    input = interface
    type = BreakBoundaryOnSubdomainGenerator
  []
[]

[Variables]
  [Mobile]
   block = 1
   order = SECOND
   []
  [m2]
    block = 2
    order = SECOND
  []
[]

[AuxVariables]
  [Solute]
    order = SECOND
  []
[]

[AuxKernels]
 # [total_solute]
 #   type = ParsedAux
 #   variable = Solute
 #   args= 'm2 Mobile'
 #   function = '(m2 + Mobile) * 6.3e28'
 # []
  [total_solute1]
    type = ParsedAux
    variable = Solute
    args= 'Mobile'
    function = '(Mobile) * 6.3e28'
    block = 1
  []
  [total_solute2]
    type = ParsedAux
    variable = Solute
    args= 'm2'
    function = '(m2 ) * 6.3e28'
    block = 2
  []
[]

[Kernels]
  [time1]
    type = ADTimeDerivative
    variable=Mobile
  []
  [diffusion1]
    type = ADMatDiffusion
    variable = Mobile
    diffusivity = D1
  []
    [diffusion2]
    type = ADMatDiffusion
    variable = m2
    diffusivity = D2
  []
  [time2]
    type = ADTimeDerivative
    variable=m2
  []
[]

[InterfaceKernels]
  [chemical]
    type = ADChemicalPotentialInterface
    variable = Mobile
    neighbor_var = m2
    boundary = interface
    s = S1
    s_neighbor = S2
    rho = 1
    rho_neighbor = 1
  []
  [diffusion]
    type = ADMatInterfaceDiffusion
    variable = Mobile
    neighbor_var = m2
    boundary = interface
    D = 'D1'
    D_neighbor = 'D2'
    rho = 1
    rho_neighbor = 1
  []
[]

[BCs]
  [left]
    type = ADDirichletBC
    variable = Mobile
    value = 1.587e-9
    boundary = 'left'
  []
  [right]
    type = ADDirichletBC
    variable = m2
    value = 0
    boundary = 'right'
  []
[]

[Functions]
  [D1] 
    type = ParsedFunction
    vars = 'D0 E kb T'
    vals = '6.6e-7 0.39 8.617e-5 600'
    value = 'D0 * exp( -E / (kb * T) )'
  []
  [D2] 
    type = ParsedFunction
    vars = 'D0 E kb T'
    vals = '1.5e-7 0.15 8.617e-5 600'
    value = 'D0 * exp( -E / (kb * T) )'
  []
  [S1] 
    type = ParsedFunction
    vars = 's0 E kb T'
    vals = '3.14e24 0.57 8.617e-5 600'
    value = 's0 * exp( -E / (kb * T) )'
  []
  [S2] 
    type = ParsedFunction
    vars = 's0 E kb T'
    vals = '6.13e20 0.25 8.617e-5 600'
    value = 's0 * exp( -E / (kb * T) )'
  []
[]

[Materials]
  [D1]
    type = ADGenericFunctionMaterial
    prop_names = D1
    prop_values = D1
    block = 1
  []
  [S1]
    type = ADGenericFunctionMaterial
    prop_names = S1
    prop_values = S1
    block = 1
  []
  [D2]
    type = ADGenericFunctionMaterial
    prop_names = D2
    prop_values = D2
    block = 2
  []
  [S2]
    type = ADGenericFunctionMaterial
    prop_names = S2
    prop_values = S2
    block = 2
  []
[]

[Executioner]
  steady_state_detection=true
  type = Transient
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  #nl_rel_tol = 1e-13
  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 3
    cutback_factor = 0.8
    growth_factor = 1.2
    dt = 100
  []
  nl_abs_tol = 1e-25
  end_time = 1e6
[]

[Preconditioning]
  [p]
    type=SMP
    full = true
  []
[]

[Postprocessors]
  [flux]
    type = ADSideDiffusiveFluxIntegral
    variable = Solute
    boundary = right
    diffusivity = D2
  []
[]

[Outputs]
  exodus = false
  csv = true
[]

#[Debug]
#  show_var_residual_norms = true
#  show_reporters = true
#[]
