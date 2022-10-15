[Mesh]
  type = GeneratedMesh
  dim = 1 
  nx = 200
  xmax = 1e-5
  elem_type = EDGE3
[]

[Problem]
  type = FEProblem # This is the "normal" type of Finite Element Problem in MOOSE
  coord_type = XYZ # cartesian
[]


[Functions]
  [ntrap_1]
    type = ParsedFunction
    value = 'scale * exp( -0.5 * ((x - mean) / sd)^2)'
    vars = 'scale mean sd'
    vals = '1e-4 2e-6 1e-6'
    # value = 'x'
  []
  [ntrap_2]
    type = ParsedFunction
    value = 'scale * exp( -0.5 * ((x - mean) / sd)^2)'
    vars = 'scale mean sd'
    vals = '2e-4 4e-6 1e-6'
    # value = '2*x'
  []
  [ntrap_3]
    type = ParsedFunction
    value = 'scale * exp( -0.5 * ((x - mean) / sd)^2)'
    vars = 'scale mean sd'
    vals = '3e-4 6e-6 1e-6'
    # value = '3*x'
  []
  [ntrap_4]
    type = ParsedFunction
    value = 'scale * exp( -0.5 * ((x - mean) / sd)^2)'
    vars = 'scale mean sd'
    vals = '4e-4 8e-6 1e-6'
    # value = '4*x'
  []
#  [trap_density]
#    type = ParsedFunction
#    value = '0.2 * (x / 5e-5)'
#  []
[]


[AuxVariables]
  [Temperature]
    initial_condition = 500
  []
[]

[Modules/Achlys/FosterMcNabb]
  [mat]
    n = 'ntrap_1 ntrap_2 ntrap_3 ntrap_4 3e-4'
    v0 = '1.0E13 1.0E13 1.0E13 1.0E13 1e13'
    E = '1.0 1.1 1.2 1.3 0.9'
    #n = 'trap_density'
    #v0 = '1e13'
    #E = '1.0'
    energy_units=eV
    D0 = 6e-7
    Ed = 0.3
    lambda = 1.1e-10
    n_sol = 6
    Temperature = "Temperature"
    variable_order= SECOND
    jsonify = 'action.json'
    materials_to_exodus = true
  []
[]

[BCs]
  [left_face]
    type = ADDirichletBC
    variable = mobile
    boundary = left
    value = 1e-3
  []
  [right_face]
    type = ADDirichletBC #DiffusionFluxBC
    variable = mobile
    boundary = right
    value = 0
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON 
  scheme = bdf2
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu  NONZERO'
  dt = 1e-4
  num_steps = 250
[]

[Postprocessors]
  [back_flux]
   type = ADSideDiffusiveFluxIntegral
    variable = mobile
    boundary = right
    diffusivity = D
  []
[]

[Outputs]
  console = true
  exodus = true
  csv = true
[]

