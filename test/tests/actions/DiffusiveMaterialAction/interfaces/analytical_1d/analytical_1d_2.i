[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 100
    xmax = 1
    elem_type = EDGE3
  []

  # assign two subdomains
  [block1]
    type = SubdomainBoundingBoxGenerator
    input = generated
    block_id = 1
    bottom_left = '0 0 0'
    top_right = '0.4 0 0'
  []
  [block2]
    type = SubdomainBoundingBoxGenerator
    input = block1
    block_id = 2
    bottom_left = '0.4 0 0'
    top_right = '1 0 0'
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


[AuxVariables]
  [Temperature]
    initial_condition = 300
  []
#  [mobile]
#    initial_condition = 0.0
#  []
[]


#[AuxKernels]
#  [total_solute1]
#    type = ParsedAux
#    variable = mobile
#    args= 'mobile_1'
#    function = '(mobile_1)'
#    block = 1
#  []
#  [total_solute2]
#    type = ParsedAux
#    variable = mobile
#    args= 'mobile_2'
#    function = '(mobile_2 )'
#    block = 2
#  []
#[]

[Variables]
  [mobile_1]
   block = 1
   order = SECOND
   []
  [mobile_2]
    block = 2
    order = SECOND
  []
[]

[Kernels]
  [diffusion]
    type = ADMatDiffusion
    variable = mobile_1
    diffusivity = D
  []
    [diffusion2]
    type = ADMatDiffusion
    variable = mobile_2
    diffusivity = D
  []
  [source1]
    type = ADBodyForce
    variable = mobile_1
    value = 2.449
  []
  [source2]
    type = ADBodyForce
    variable = mobile_2
    value = 2.449
  []

[]

[InterfaceKernels]
  [chemical]
    type = ADChemicalPotentialInterface
    variable = mobile_1
    neighbor_var = mobile_2
    boundary = interface
    s = S
    s_neighbor = S
    rho = rho
    rho_neighbor = rho
  []
[./diffusion]
  type = ADMatInterfaceDiffusion
  variable = mobile_1
  neighbor_var = mobile_2
  boundary = interface
  D = 'D'
  D_neighbor = 'D'
  rho = rho
  rho_neighbor = rho
[../]
[]

[BCs]
  [left]
    type = ADDirichletBC
    variable = mobile_1
    value = 1
    boundary = 'left'
  []
  [right]
    type = ADDirichletBC
    variable = mobile_2
    value = 0.25
    boundary = 'right'
  []
[]

[Materials]
  [left]
    type = ADGenericConstantMaterial
    prop_names = 'D S rho'
    prop_values = '1 2 1'
    block = 1
    outputs = 'exodus'
  []
  [right]
    type = ADGenericConstantMaterial
    prop_names = 'D S rho'
    prop_values = '1.5 1 1'
    block = 2
    outputs = 'exodus'
  []
[]

[Executioner]
  type = Steady
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  nl_rel_tol = 1e-13
[]

[Outputs]
  exodus = true
[]
