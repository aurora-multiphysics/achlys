[Mesh]
  [surface_mesh]  
    type = GeneratedMeshGenerator
    dim = 1
    xmin = 0
    xmax =1e-6
    nx = 1
  []
  [bulk_mesh]      
    type = GeneratedMeshGenerator   
    dim=1
    xmin=1e-6
    xmax=1.5e-6
    bias = 1.003
    nx=50
  []
  [Mesh_stitcher]  
    type = StitchedMeshGenerator
    inputs = 'surface_mesh bulk_mesh'
    clear_stitched_boundary_ids = true
    stitch_boundaries_pairs = 'right left'
    parallel_type = 'replicated'
  []
  [subdomain_gas] 
    input=Mesh_stitcher
    type=SubdomainBoundingBoxGenerator
    bottom_left='0 0 0'
    top_right='1e-6 0 0'
    block_id=0
  []
  [subdomain_wall] 
    input=subdomain_gas
    type=SubdomainBoundingBoxGenerator
    bottom_left='1e-6 0 0'
    top_right='1.5e-6 0 0'
    block_id=1
  []
  [gas-wall_boundary] 
    type=SideSetsBetweenSubdomainsGenerator
    input=subdomain_wall
    primary_block='0'
    paired_block='1'
    new_boundary='interface'
  []
[]

[Variables]
  [Mobile]
    initial_condition = 1e-15
    block = 1
#    order = SECOND
  []
  [Surface]
    initial_condition = 0.9
    block = 0
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxVariables]
  [Temperature]
   initial_condition = 350
    family = MONOMIAL
    order = CONSTANT
  []
  [Pressure]
    initial_condition = 1.0e6
    family = MONOMIAL
    order = CONSTANT
  []
[]




[Kernels]
  [./H3_diffusion_eq1]
    type = ADMatDiffusion
    variable = Mobile
    diffusivity = D
  [../]
  [surface_kernel]
    type = ADSurfaceAdsorption
    variable = Surface 
    Gamma = Gamma
  []
[]

[Materials]
   [bulk]
    type = TrappingMaterial
# Energies
    E_diff = 0.39
    E1 = 0.87
    E2 = 1.0
    E3 = 0.0
    k_boltz = 8.617333E-5
# pre-exponential rate constants
    v1 = 1e13
    v2 = 1e13
    v3 = 1e13
    D0 = 4.1e-7
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
    block = 1
  [../]
  [surface]
    type = ADAdsorbingMaterial
    v0_d = 1e13
    v0_sb = 1e13
    v0_bs = 1e13
    E_r = 0.2
    E_d = 0.69
    E_a = 1.33
    k_boltz = 8.617333E-5
    lambda = 1.1e-10
    n_sol = 6
    rho = 6.3e28
    mass = 6.69e-27 
    sigma = 0 
    s = 0.1
    block = 0
    Temperature = Temperature
    Pressure = Pressure
  []
[]

[InterfaceKernels]
  [adsorption]
    type = ADSurfaceInterface
    vd = Vd
    vbs = Vbs
    vsb = Vsb
    sigma = Sigma
    variable = Surface
    neighbor_var = Mobile
    D_neighbor = D
    boundary = interface
  []
[]


[Postprocessors]
  [Cm]
    type = PointValue
    variable = Mobile
    point = '1e-6 0 0'
  []  
  [theta]
    type = PointValue
    variable = Surface
    point = '1e-6 0 0'
  []
[]

[Executioner]
  scheme = bdf2
  type = Steady #Transient
  solve_type = NEWTON
  petsc_options_iname = '-snes_type -pc_type -ksp_type -pc_factor_shift_type'
  petsc_options_value = 'fas lu bcgs NONZERO'
  #dt = 1e-4  # seconds
  automatic_scaling = True
  compute_scaling_once = False
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-22 # This should avoid solve failures when close to steady-state
  nl_max_its = 7
[]

[Preconditioning]
  [./smp]
    type = FDP
    full = true
  [../]
[]

[Outputs]
  exodus = false
  console = false
  csv = true
[]
