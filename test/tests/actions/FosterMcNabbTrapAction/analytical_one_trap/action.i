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


[AuxVariables]
  [Temperature]
    initial_condition = 1000
  []
[]

[Modules/Achlys/FosterMcNabb]
[mat]
    n = '0.1'
    v0 = '1.0E13'
    E = '8.6e-3'
    energy_units=eV
    D0 = 1.0
    Ed = 0.0
    lambda = 3.16E-8
    n_sol = 2
    Temperature = "Temperature"
    variable_order= FIRST
    jsonify = 'action.json'
[]
[]

[ICs]
 [mobile]
   type = ConstantIC
   variable = mobile
   value = 0.0
 []
 [trapped]
   type = ConstantIC
   variable = trap_1
   value = 0.0
 []
[]

[BCs]
  [left_face]
    type = ADDirichletBC
    variable = mobile
    boundary = left
    value = 1e-4
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
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu  NONZERO'
  dt = 1e-10
  num_steps = 100
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
  console = false
  exodus = true
  csv = true
[]

