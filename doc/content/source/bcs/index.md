!config navigation breadcrumbs=True scrollspy=True

# BCs


## Achlys Boundary Conditions

New boundary conditions available through the Achlys app are listed below. These mostly implement 
some form of recombination flux of the form given in [!eqref](eq:bc_recombination).

\begin{equation}
\label{eq:bc_recombination}
\varphi_{out} =  K_{r}C_{m}^2
\end{equation}

Where the recombination coefficient, $K_r$, is given by [!eqref](eq:recombination_coef).

\begin{equation}
\label{eq:recombination_coef}
K_{r}(T) =  K_0\exp\left(-E_r / k_B T\right)
\end{equation}

!! Note all default options are also available from the underlying MOOSE framework, notably for basic Dirichlet conditions etc.

!syntax list /BCs groups=achlysApp heading=Available BCs

!! !syntax list /BCs groups= MooseApp HeatConductionApp heading=Framework Options

## Common Framework BCs

See below for framework documentation on commonly used boundary conditions that are not specific
to the Achlys module. Note the full list of MOOSE BCs can be found [here](https://mooseframework.inl.gov/syntax/BCs/index.html)

| Name| Description| Link|
|:-|:-|:-|:-|
| Dirichlet | Constant value | [ADDirichletBC](https://mooseframework.inl.gov/source/bcs/ADDirichletBC.html) |
| Neumann | Constant gradient | [ADNeumannBC](https://mooseframework.inl.gov/source/bcs/NeumannBC.html) |

## Heat Transfer BCs

As is the case for all Thermal transport considerations within Achlys, all MOOSE objects are taken 
directly from either the framework or [Heat Conduction App](https://mooseframework.inl.gov/modules/heat_conduction/index.html.)

See the example below for applying thermal boundary conditions to a material where:

- A constant heat flux of $10 \text{\:MW\:m}^{-2}$ is applied tot the top surface
- A convective heat flux is applied to another surface with some specified heat flux coefficiant

!listing /test/tests/kernels/steady_heat_transfer/temperature.i block=BCs