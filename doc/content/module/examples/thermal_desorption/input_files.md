
Links to other pages for this example :
- [Overview](examples/thermal_desorption/index.md)
- [Additional theory](extrinsic_traps.md)
- [Preparing the simulation](input_files.md)

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
| Lattice sites | $N_{\text{sol}}$  | $2.0 \rho$  | $m^{-3}$ |
| Trap Density (low energy intrinsic)  | $n_1$    | $1.00\times 10^{-3}\rho$   | $m^{-3}$ |
| Trap Density (high energy intrinsic) | $n_2$    | $1.00\times 10^{-4}\rho$   | $m^{-3}$ |
| Maximum extrinsic trap Density type a | $n_{3a\_max}$    | $1.00\times 10^{-1}\rho$   | $m^{-3}$ |
| Maximum extrinsic trap Density type b | $n_{3b\_max}$    | $1.00\times 10^{-3}\rho$   | $m^{-3}$ |
| Diffusion rate pre-exponential factor | $D_0$ | $1.00\times 10^{-7}$ | $m^{2}s^{-1}$ |
| Diffusion Energy | $E_{\text{diff}}$ | 0.39 | $eV$ |
| Lattice constant | $\lambda$ | $1\times 10^{-9}$ | $m$ |
| De-trapping pre-exponential factor (all traps) | $\nu_{1,2,3}$ |  $1.0\times 10^{-13}$ | $s^{-1}$ |
| De-trapping Energy, trap 1 | $E_{1}$ | $0.85$ | $eV$ |
| De-trapping Energy, trap 2 | $E_{2}$ | $1.00$ | $eV$ |
| De-trapping Energy, trap 3 | $E_{3}$ | $1.45$ | $eV$ |
| Temperature (implantation and resting) |  $T_{\text{imp}}$ |  $300$ | $K$ |
| Temperature ramping rate |  $\beta$ |  $8$ | $K/s$ |
| Simulation time | $t_{\text{sim}}$ | $1.0\times 10^{-8}$ | $s$ |
| Extrinsic trap creation rate | $\mu_a$    | $1.00\times 10^{-2}$   | $s^{-1}$ |

Also common between an calculation stages are the surface outflow conditions, for which a dirichlet
condition with a zero value is applied to all external surfaces, and the core set of kernels, which 
describe the common physics for all Achlys calculaitons; the source term is the only kernel that is 
treated differently between calculations.

## Stage 1: Implantation id=implantation

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

## Stage 2: Resting id=resting

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

