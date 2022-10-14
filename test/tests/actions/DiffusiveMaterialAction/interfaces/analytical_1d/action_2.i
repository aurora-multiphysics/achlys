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
[]


[Modules/Achlys/DiffusiveMaterial]
  [block_1]
    D0 = 1.0
    Ed = 0.0
    S0 = 2.0
    Es = 0.0
    atomic_density = 1.
    Temperature = Temperature
    variable_order = SECOND
    #aux_variables = 'mobile'
    solid_boundaries = 'interface'
    generate_interface_kernels = true
    jsonify = analytical_1d_2.json
    block = 1
    materials_to_exodus = true
  []
  [block_2]
    D0 = 1.5
    Ed = 0.0
    S0 = 1.0
    Es = 0.0
    atomic_density = 1.
    Temperature = Temperature
    variable_order = SECOND
    #aux_variables = 'mobile'
    solid_boundaries = 'interface'
    generate_interface_kernels = true
    jsonify = analytical_1d_2.json
    block = 2
    materials_to_exodus = true
  []
[]

[Kernels]
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

[Executioner]
  type = Steady
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  nl_rel_tol = 1e-13
[]

[Outputs]
  exodus = true
[]
