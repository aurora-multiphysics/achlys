[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 5
    xmax = 1
    ymax = 1
    elem_type = QUAD9
  []

  # assign two subdomains
  [block1]
    type = SubdomainBoundingBoxGenerator
    input = generated
    block_id = 1
    bottom_left = '0 0 0'
    top_right = '0.5 1 0'
  []
  [block2]
    type = SubdomainBoundingBoxGenerator
    input = block1
    block_id = 2
    bottom_left = '0.5 0 0'
    top_right = '1 1 0'
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
    [InitialCondition]
      type = FunctionIC
      function = cm1
    []
     block = 1
     order = SECOND
   []
  [m2]
    [InitialCondition]
      type = FunctionIC
      function = cm2
    []
    block = 2
    order = SECOND
  []
[]


[Kernels]
  [time_1]
    type = ADTimeDerivative
    variable=Mobile
  []
  [time_2]
    type = ADTimeDerivative
    variable=m2
  []
  [diffusion]
    type = ADMatDiffusion
    variable = Mobile
    diffusivity = D1
  []
    [diffusion2]
    type = ADMatDiffusion
    variable = m2
    diffusivity = D2
  []
  [f1]
    type = ADBodyForce
    variable = Mobile
    value = 1
    function = F1
  []
  [f2]
    type = ADBodyForce
    variable = m2
    value = 1
    function = F2
  []

[]

[InterfaceKernels]
  [chemical]
    type = ADChemicalPotentialInterface
    variable = Mobile
    neighbor_var = m2
    boundary = interface
    s = S1
    s_neighbour = S2
  []
  [diffusion]
    type = ADMatInterfaceDiffusion
    variable = Mobile
    neighbor_var = m2
    boundary = interface
    D = 'D1'
    D_neighbor = 'D2'
  [../]
[]

[BCs]
  [Mobile]
    type = ADFunctionDirichletBC
    variable = Mobile
    boundary = 'left right top bottom'
    function = cm1
  []
  [m2]
    type = ADFunctionDirichletBC
    variable = m2
    boundary = 'left right top bottom'
    function = cm2
  []
[]

[Functions]
  [T]
    type = ParsedFunction
    value = '500 + 30*cos(2*pi*x)*cos(2*pi*y)*cos(2*pi*t)'
  []
  [dTdt]
    type = ParsedFunction
    value = '-60 * pi * cos(2*pi*x) * cos(2*pi*y) * sin(2*pi*t)'
  []
  [grad_T_x]
    type = ParsedFunction
    value = '-60 * pi * sin(2*pi*x) * cos(2*pi*y) * cos(2*pi*t)'
  []
  [grad_T_y]
    type = ParsedFunction
    value = '-60 * pi * cos(2*pi*x) * sin(2*pi*y) * cos(2*pi*t)'
  []
  [grad2_T]
    type = ParsedFunction
    value = '-240 * pi * pi * cos(2*pi*x) * cos(2*pi*y) * cos(2*pi*t)'
  []

  [S1]
    type = ParsedFunction
    vars = 's1 e1 kb T'
    vals = '1 0.1 8.617e-5 T'
    value = 's1 * exp( (-e1) / (kb * T) )'
  []
  [S2]
    type = ParsedFunction
    vars = 's2 e2 kb T'
    vals = '2 0.2 8.617e-5 T'
    value = 's2 * exp( (-e2) / (kb * T) )'
  []

  [Rs] # The ratio s2/s1
    type = ParsedFunction
    vars = 's1 s2 e1 e2 kb T'
    vals = '1 2 0.1 0.2 8.617e-5 T'
    value = 's2/s1 * exp( (e1- e2) / (kb * T) )'
  []
  [dRsdT]
    type = ParsedFunction
    vars = 'e1 e2 kb T Rs'
    vals = '0.1 0.2 8.617e-5 T Rs'
    value = '( (e2-e1) / (kb * T * T) ) * Rs '
  []
  [dRsdt]
    type = ParsedFunction
    vars = 'dRsdT dTdt'
    vals = 'dRsdT dTdt'
    value = 'dRsdT * dTdt'
  []
  [d2RsdT2]
    type = ParsedFunction
    vars = 'e1 e2 kb T Rs'
    vals = '0.1 0.2 8.617e-5 T Rs'
    #value =  '( (e1-e2) * ( e1 - e2 +(2*kb*T) ) / (kb^2 * T^4) ) * Rs'
    value =  '( (e1-e2) * ( e1 - e2 +(2*kb*T) ) / (kb^2 * T^4) ) * Rs'
  []
  [grad_Rs_x]
    type = ParsedFunction
    vars = 'dRsdT grad_T_x '
    vals = 'dRsdT grad_T_x'
    value = 'dRsdT * grad_T_x'
  []
  [grad_Rs_y]
    type = ParsedFunction
    vars = 'dRsdT grad_T_y '
    vals = 'dRsdT grad_T_y'
    value = 'dRsdT * grad_T_y'
  []
  [grad2_Rs]
    type = ParsedFunction
    vars = 'dRsdT grad_T_x grad_T_y d2RsdT2 grad2_T'
    vals = 'dRsdT grad_T_x grad_T_y d2RsdT2 grad2_T'
    value = '(grad_T_x * grad_T_x * d2RsdT2) + (dRsdT * grad2_T) + (grad_T_y * grad_T_y * d2RsdT2)' # + (dRsdT * grad2_T)'
  []

  [D1] 
    type = ParsedFunction
    vars = 'D0 E kb T'
    vals = '1 0.1 8.617e-5 T'
    value = 'D0 * exp( -E / (kb * T) )'
  []
  [dD1dT] 
    type = ParsedFunction
    vars = 'E kb T D1'
    vals = '0.1 8.617e-5 T D1'
    value = '(E / (kb * T * T)) * D1'
  []
  [grad_D1_x] 
    type = ParsedFunction
    vars = 'dD1dT grad_T_x'
    vals = 'dD1dT grad_T_x'
    value = 'dD1dT * grad_T_x'
  []
  [grad_D1_y] 
    type = ParsedFunction
    vars = 'dD1dT grad_T_y'
    vals = 'dD1dT grad_T_y'
    value = 'dD1dT * grad_T_y'
  []

  [D2] 
    type = ParsedFunction
    vars = 'D0 E kb T'
    vals = '2 0.2 8.617e-5 T'
    value = 'D0 * exp( -E / (kb * T) )'
  []
  [dD2dT] 
    type = ParsedFunction
    vars = 'E kb T D2'
    vals = '0.2 8.617e-5 T D2'
    value = '(E / (kb * T * T)) * D2'
  []
  [grad_D2_x] 
    type = ParsedFunction
    vars = 'dD2dT grad_T_x'
    vals = 'dD2dT grad_T_x'
    value = 'dD2dT * grad_T_x'
  []
  [grad_D2_y] 
    type = ParsedFunction
    vars = 'dD2dT grad_T_y'
    vals = 'dD2dT grad_T_y'
    value = 'dD2dT * grad_T_y'
  []

  [cm1]
    type = ParsedFunction
    value = '2 + cos(2*pi*x)*cos(2*pi*y) + t'
  []
  [dcm1dt] 
    type = ParsedFunction
    value = 1
  []
  [grad_cm1_x] 
    type = ParsedFunction
    value = '-2 * pi * sin(2*pi*x) * cos(2*pi*y)'
  []
  [grad_cm1_y] 
    type = ParsedFunction
    value = '-2 * pi * cos(2*pi*x) * sin(2*pi*y)'
  []
  [grad2_cm1] 
    type = ParsedFunction
    value = '-8 * pi * pi * cos(2*pi*x) * cos(2*pi*y)'
  []

  [F1] 
    type = ParsedFunction
    vars = 'dcm1dt grad_D1_x grad_D1_y grad_cm1_x grad_cm1_y D1 grad2_cm1'
    vals = 'dcm1dt grad_D1_x grad_D1_y grad_cm1_x grad_cm1_y D1 grad2_cm1'
    value = 'dcm1dt - ( (grad_D1_x * grad_cm1_x) + (grad_D1_y * grad_cm1_y) + (D1 * grad2_cm1) )'
  []


  [cm2]
    type = ParsedFunction
    vars = 'Rs cm1'
    vals = 'Rs cm1'
    value = 'Rs * cm1'
  []
  [dcm2dt] 
    type = ParsedFunction
    vars = 'dRsdt cm1  dcm1dt Rs'
    vals = 'dRsdt cm1  dcm1dt Rs'
    value = '(dRsdt * cm1) + (dcm1dt *Rs)'
  []

  [G] # diffusive term for f2 -- lots of product rule
    type = ParsedFunction
    vars = 'D2 Rs cm1 grad2_cm1 grad2_Rs grad_cm1_x grad_cm1_y grad_D2_x grad_D2_y grad_Rs_x grad_Rs_y'
    vals = 'D2 Rs cm1 grad2_cm1 grad2_Rs grad_cm1_x grad_cm1_y grad_D2_x grad_D2_y grad_Rs_x grad_Rs_y'
    value = '(D2*Rs*grad2_cm1) + (D2*cm1*grad2_Rs) + Rs*((grad_D2_x*grad_cm1_x) + (grad_D2_y * grad_cm1_y)) + D2*((grad_Rs_x*grad_cm1_x) + (grad_Rs_y*grad_cm1_y)) + cm1*((grad_D2_x*grad_Rs_x) + (grad_D2_y * grad_Rs_y)) + D2*((grad_Rs_x*grad_cm1_x) + (grad_Rs_y * grad_cm1_y))'
  []


  #[grad_cm2] 
  #  type = ParsedFunction
  #  vars = 'grad_Rs cm1 Rs grad_cm1'
  #  vals = 'grad_Rs cm1 Rs grad_cm1'
  #  value = '(grad_Rs * cm1 ) + (Rs * grad_cm1)'
  #[]
  #[grad2_cm2] 
  #  type = ParsedFunction
  #  vars = 'grad_Rs cm1 Rs grad_cm1 grad2_cm1 grad2_Rs'
  #  vals = 'grad_Rs cm1 Rs grad_cm1 grad2_cm1 grad2_Rs'
  #  value = '(grad2_Rs * cm1 ) + (2* grad_Rs * grad_cm1) + (Rs * grad2_cm1)'
  #[]

  #[F1] 
  #  type = ParsedFunction
  #  vars = 'dcm1dt grad_D1 grad_cm1 D1 grad2_cm1'
  #  vals = 'dcm1dt grad_D1 grad_cm1 D1 grad2_cm1'
  #  value = 'dcm1dt - ( (grad_D1 * grad_cm1) + (D1 * grad2_cm1) )'
  #[]
  [F2] 
    type = ParsedFunction
    vars = 'dcm2dt G'
    vals = 'dcm2dt G'
    value = 'dcm2dt - G'
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
  [left]
    type = ADGenericConstantMaterial
    prop_names = 'rho rho_neighbour'
    prop_values = '1 1'
    block = 1
  []
  [right]
    type = ADGenericConstantMaterial
    prop_names = 'rho rho_neighbour'
    prop_values = '1 1'
    block = 2
  []
[]

[Executioner]
  type = Transient
  scheme = bdf2
  solve_type = NEWTON
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  #nl_rel_tol = 1e-13
  dt = 5e-3
  end_time = 0.04
[]

[Outputs]
  exodus = true
[]
