
Links to other pages for this example :
- [Overview](examples/thermal_desorption/index.md)
- [Additional theory](extrinsic_traps.md)
- [Preparing the simulation](module/examples/thermal_desorption/input_files.md)

# Input file preparation

This calculation is carried out in 3 stages

- [Implantation](#implantation): the material sample is bombarded with deuterium ions at constant temperature
- [Resting](#resting): the source term is switched off, slow off-gassing occurs due to the low temperature
- [Desorption](#desorption): Temperature is raised at a constant rate and the rate of desorption is measured from each surface

## Multiapp 

For this example a multiapp structure has been used to combine the three simulation steps. 
The final, desorption step is the dominant process and each simulation step launches the step 
which precedes it before beginning its own calculation. 
The transfer block is used to copy the results from variables in sub-app calculations back to the calling app.


!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=MultiApps Transfers

## Common parameters

The parameters used for this example have been taken from [!cite](DELAPORTE2019), 
the experimental set-up follows [!cite](OGOROD2003)

!table id=table:input_parameters caption=Example innput parameters
| parameter name  | symbol   | value | Unit |
|:-|:-|:-|:-|
| Density | $\rho$ | $6.3 \times 10^{28}$ | $m^{-3}$ |
| Lattice sites | $N_{\text{sol}}$  | $6.0 \rho$  | $m^{-3}$ |
| Trap Density (low energy intrinsic)  | $n_1$    | $1.00\times 10^{-3}\rho$   | $m^{-3}$ |
| Trap Density (high energy intrinsic) | $n_2$    | $4.00\times 10^{-4}\rho$   | $m^{-3}$ |
| Maximum extrinsic trap Density type a | $n_{3a\_max}$    | $1.00\times 10^{-1}\rho$   | $m^{-3}$ |
| Maximum extrinsic trap Density type b | $n_{3b\_max}$    | $1.00\times 10^{-2}\rho$   | $m^{-3}$ |
| Diffusion rate pre-exponential factor | $D_0$ | $1.00\times 10^{-7}$ | $m^{2}s^{-1}$ |
| Diffusion Energy | $E_{\text{diff}}$ | 0.39 | $eV$ |
| Lattice constant | $\lambda$ | $1\times 10^{-9}$ | $m$ |
| De-trapping pre-exponential factor (all traps) | $\nu_{1,2,3}$ |  $1.0\times 10^{-13}$ | $s^{-1}$ |
| De-trapping Energy, trap 1 | $E_{1}$ | $0.87$ | $eV$ |
| De-trapping Energy, trap 2 | $E_{2}$ | $1.00$ | $eV$ |
| De-trapping Energy, trap 3 | $E_{3}$ | $1.5$ | $eV$ |
| Temperature (implantation and resting) |  $T_{\text{imp}}$ |  $300$ | $K$ |
| Temperature ramping rate |  $\beta$ |  $8$ | $K/s$ |
| Simulation time | $t_{\text{sim}}$ | $1.0\times 10^{-8}$ | $s$ |
| Extrinsic trap creation rate | $\mu_a$    | $6.00\times 10^{-4}$   | $s^{-1}$ |
| Extrinsic trap creation rate | $\mu_b$    | $2.00\times 10^{-4}$   | $s^{-1}$ |
| Plastic deformation region | $x_p$ | $1.0\times 10^{-6}$ | $m$ |

Also common between all calculation stages are:

- A zero valued dirichlet condition is applied for the mobile phase at all external surfaces
- The core set of kernels are identical for all Achlys calculations. Though here the external source term is only used for the implantation phase.
- Material properties as described in the table above
- The problem geometry and mesh


## Stage 1: Implantation id=implantation

In this simulation:

- A gaussian implantation profile is used for the source term
- A dirichlet boundary condition with a zero value for the surface  concentration of the mobile phase is used for all external surfaces
- Constant values are used for the densities of two instrisic trap types, labelled trap 1 and trap2. These are uniformly distributed throughout the entire material domain.
- A third trap is simulated whose concentration increases over time as the material is irradiated. This trap only appears in a narrow region near the surface exposed to particle bombardment. 
- The temperature is kept at a constant 300 K
- The total simulation time is 400 s

### Mesh

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=Mesh

### Source term: the implantation profile

The implantation profile for the experiment is a function of the composition, energy, and direction of
the incident particles. This is generally determined by an external tool such as SRIM.
Here a gaussian profile with mean 4.5 nm and standard deviation of 4.5 nm has been used 
following [!cite](DELAPORTE2019). The scale factor applied means units of $m^{-3}$ are obtained
for the source term when the value of the gaussian function is multiplied by the particle flux in $m^{-2}$.

It is also possible to define other profiles (exonweib shown below) or alternatively to read in 
values from a CSV file.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=Functions

### Kernels

The kernels shown below represent the standard set for any Achlys calculation. Note only that the 
source term has been selected to use the gaussian implantation profile defined above. The "value" field
here is the value of the particle flux which has been scaled by the density of solute atoms.
For the case of deuterium in tungsten the scale factor is $6.3 \times 10^{28}$ and the 
units are $(D/W)m^{-2}$.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=Kernels

### Material Properties

A specific material class has been developed to model the transient evolution of extrinsic traps
that is used in this calculation. The example inputs for the 
[ExtrinsicTransientTrappingMaterial](source/materials/ExtrinsicTransientTrappingMaterial2.md) class
are shown below.

Using this class means that the density of availble type-3 traps increases over time within this
simulation stage. The rate of increase depends on the flux, whose shape is defined by the "function" parameter
and whose magnitude is given by the "flux" field.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=Materials

### Boundary Conditions

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=BCs

### Executioner

There are 3 things to note within this input block:

- The total simulation time is set by the end_time parameter. This is where we specify the duration of the implantation phase
- Adaptive timestepping is used to achieve the most efficient time-steps.
- PetSc allows any numerical scheme to be substituted here to carry out the solve. Here a bi-conjugate gradient linear method with LU preconditioner and second-order time integration has been used.


!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=Executioner

### Visualising implantation

The implantation step of this calculation can be run as a standalone
simulation. The input file can be run by passing it's path using the "-i" flag 
as shown below; outputs will be written to the same folder in which
the input is located.

```
achlys_exe=~/projects/achlys/achlys-opt
input_file=~/projects/achlys/problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i
./achlys-opt -i $input_file
```

Paraview can be used to visualise the resulting exodus file. 
in addition the total quantities of deuterium in each phase 
will be written to a csv file at each timestep. 


## Stage 2: Resting id=resting

In this stage:

- The source term is switched off so no new particles are implanted
- The creation of new type-3 traps also stops when the implantation flux stops meaning the concentration and distribution of traps remains static from the end of implantation
- Desorption continues to occur from the external surfaces, though this is limited due to the low temperature; the boundary condition remains a zero-valued dirichlet condition
- The temperature is a constant 300 K
- The total resting time is 50 s

### Kernels

The Kernels here are the standard set for Achlys calculations. 
Note only that the source term has now been removed for the resting phase.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/resting_multi.i block=Kernels

### Material Properties

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/resting_multi.i block=Materials

### Boundary Conditions

During resting the mobile concentration at the surface is considered to be zero.
[Dirichlet conditions](https://mooseframework.inl.gov/source/bcs/ADDirichletBC.html)
are applied at both surfaces.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/resting_multi.i block=BCs

## Stage 3: Desorption id=desorption

In this stage:

- The temperature is raised at a constant rate of 8 K/s.
- The boundary condition remains a zero-valued dirichlet condition.
- The concentration and distribution of traps remains static. This state is unchanged from the end of the implantation calculation.
- The rate of desorption from each surface is measured using the ADSideFluxIntegral postprocessor and results are written to a CSV file.

### Kernels

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=Kernels

### Material Properties

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=Materials

### Boundary Conditions

For the desorption phase the mobile concentration at the surface is considered to be zero. 
[Dirichlet conditions](https://mooseframework.inl.gov/source/bcs/ADDirichletBC.html) 
are applied at both surfaces.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=BCs

### Outputs

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=Postprocessors Outputs

