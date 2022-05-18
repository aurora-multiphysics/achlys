[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 200
    xmax = 4e-3
  []

  # assign two subdomains
  [block1]
    type = SubdomainBoundingBoxGenerator
    input = generated
    block_id = 1
    bottom_left = '0 0 0'
    top_right = '2e-3 0 0'
    block_name = W
  []
  [block2]
    type = SubdomainBoundingBoxGenerator
    input = block1
    block_id = 2
    bottom_left = '2e-3 0 0'
    top_right = '4e-3 0 0'
    block_name = Cu
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

[Modules/Achlys/FosterMcNabb]
  [W]
    n = '1e-3 1e-4 5e-2 7e-4'
    v0 = '1.0E13 1e13 1e13 1e13'
    E = '0.5 0.6 0.7 0.8'
    D0 = 2.4e-7
    Ed = 0.39
    S0 = 1.87e24
    Es = 1.04
    lambda = 1.1e-10
    n_sol = 2
    atomic_density = 6.3e28
    Temperature = "Temperature"
    block = W
    aux_variables = 'mobile retention'
    solid_boundaries = 'interface'
    generate_interface_kernels = true
    jsonify = steady_out.json
  []
  [Cu]
    n = '1e-4'
    v0 = '1.0E13'
    E = '0.87'
    D0 = 6.6e-7
    Ed = 0.39
    S0 = 3.14e24
    Es = 0.57
    lambda = 1.1e-10
    n_sol = 2
    Temperature = "Temperature"
    block = Cu
    atomic_density = 6.3e28
    aux_variables = 'mobile retention'
    solid_boundaries = 'interface'
    generate_interface_kernels = true
    jsonify = steady_out.json
  []
[]

[AuxVariables]
  [Solute]
  []
  [Temperature]
    initial_condition = 500
  []
[]

#[AuxKernels]
 # [total_solute]
 #   type = ParsedAux
 #   variable = Solute
 #   args= 'm2 Mobile'
 #   function = '(m2 + Mobile) * 6.3e28'
 # []
 # [total_solute1]
 #   type = ParsedAux
 #   variable = Solute
 #   args= 'mobile_1'
 #   function = '(mobile_1) * 6.3e28'
 #   block = 1
 # []
 # [total_solute2]
 #   type = ParsedAux
 #   variable = Solute
 #   args= 'mobile_2'
 #   function = '(mobile_2 ) * 6.3e28'
 #   block = 2
 # []
#[]



#[InterfaceKernels]
#  [chemical]
#    type = ADChemicalPotentialInterface
#    variable = mobile_1
#    neighbor_var = mobile_2
#    boundary = interface
#    s = S
#    s_neighbour = S
#    rho_neighbour = rho
#  []
#  [diffusion]
#    type = ADMatInterfaceDiffusion
#    variable = mobile_1
#    neighbor_var = mobile_2
#    boundary = interface
#    D = 'D'
#    D_neighbor = 'D'
#    rho_neighbour = rho
#  []
#[]

[BCs]
  [left]
    type = ADDirichletBC
    variable = mobile_W
    value = 1.587e-9
    boundary = 'left'
  []
  [right]
    type = ADDirichletBC
    variable = mobile_Cu
    value = 0
    boundary = 'right'
  []
[]


[Executioner]
  type = Steady
  solve_type = NEWTON
  #petsc_options_iname = '-pc_type -pc_factor_shift_type'
  #petsc_options_value = 'lu NONZERO'
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  #nl_rel_tol = 1e-13
[]

[Outputs]
  exodus = false #true
  console = false #true
[]
