[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 50
    ny = 50
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

  [G1] # diffusive term for f1 -- parsed from wolfram output
    type = ParsedFunction
    vars = 'D Ed k'
    vals = '2 0.2 8.617e-5'
    value = '-8*pi^2*D*cos(2*pi*x)*cos(2*pi*y)*exp(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))+(120*pi^2*Ed*D*cos(2*pi*t)*sin(2*pi*x)^2*cos(2*pi*y)^2*exp(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500))))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)+(120*pi^2*Ed*D*cos(2*pi*t)*cos(2*pi*x)^2*sin(2*pi*y)^2*exp(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500))))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)'
  []
  [G2] # diffusive term for f2 -- parsed from wolfram output
    type = ParsedFunction
    vars = 'D Ed R e1 e2 k' 
    vals = '2 0.2 2 0.1 0.2 8.617e-5'
    value = '-(60*Ed*D*e^(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi*cos(2*pi*t)*cos(2*pi*y)*sin(2*pi*x)*((60*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi*cos(2*pi*t)*cos(2*pi*y)*(t+cos(2*pi*x)*cos(2*pi*y)+2)*sin(2*pi*x))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)-2*R*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi*cos(2*pi*y)*sin(2*pi*x)))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)+D*e^(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*(-(240*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)*cos(2*pi*y)^2*sin(2*pi*x)^2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)+(7200*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)^2*cos(2*pi*y)^2*(t+cos(2*pi*x)*cos(2*pi*y)+2)*sin(2*pi*x)^2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^3)+(3600*R*(e1-e2)^2*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)^2*cos(2*pi*y)^2*(t+cos(2*pi*x)*cos(2*pi*y)+2)*sin(2*pi*x)^2)/(k^2*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^4)-4*R*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*x)*cos(2*pi*y)+(120*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)*(t+cos(2*pi*x)*cos(2*pi*y)+2))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2))-(60*Ed*D*e^(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi*cos(2*pi*t)*cos(2*pi*x)*sin(2*pi*y)*((60*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi*cos(2*pi*t)*cos(2*pi*x)*(t+cos(2*pi*x)*cos(2*pi*y)+2)*sin(2*pi*y))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)-2*R*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi*cos(2*pi*x)*sin(2*pi*y)))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)+D*e^(-Ed/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*(-(240*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)*cos(2*pi*x)^2*sin(2*pi*y)^2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2)+(7200*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)^2*cos(2*pi*x)^2*(t+cos(2*pi*x)*cos(2*pi*y)+2)*sin(2*pi*y)^2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^3)+(3600*R*(e1-e2)^2*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)^2*cos(2*pi*x)^2*(t+cos(2*pi*x)*cos(2*pi*y)+2)*sin(2*pi*y)^2)/(k^2*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^4)-4*R*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*x)*cos(2*pi*y)+(120*R*(e1-e2)*e^((e1-e2)/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)))*pi^2*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)*(t+cos(2*pi*x)*cos(2*pi*y)+2))/(k*(30*cos(2*pi*t)*cos(2*pi*x)*cos(2*pi*y)+500)^2))'
  []



 # [F1] 
 #   type = ParsedFunction
 #   vars = 'dcm1dt G1'
 #   vals = 'dcm1dt G1'
 #   value = 'dcm1dt - G1'
 # []
  [F2] 
    type = ParsedFunction
    vars = 'dcm2dt G2'
    vals = 'dcm2dt G2'
    value = 'dcm2dt - G2'
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
  type = Transient
  scheme = bdf2
  solve_type = NEWTON
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_shift_type'
  petsc_options_value = 'bcgs lu NONZERO'
  #nl_rel_tol = 1e-13
  dt = 5e-4
  end_time = 0.1
[]

[Outputs]
  exodus = true
[]
