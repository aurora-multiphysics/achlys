# ExtrinsicTransientTrappingMaterial2

!syntax description /Materials/ExtrinsicTransientTrappingMaterial2

## Overview

!! Replace these lines with information regarding the ExtrinsicTransientTrappingMaterial2 object.

This class implements the trapping material class for the case where additional extrinsic trapping sites
are created over time due to the influence of heavy particle bombardment.

Note that once a calculation is complete using this class for the creation of new traps, subsequent
calculations can use one of the complementary classes which will maintain this final state.

- [ExtrinsicStaticTrappingMaterial](source/materials/ExtrinsicStaticTrappingMaterial.md) for constant temperature cases
- [ExtrinsicStaticTrappingMaterialRamptingT](source/materials/ExtrinsicStaticTrappingMaterialRampingT.md) for cases with a constant temperature ramp, for example in the case of thermal desorption

## Basic material properties

As with all trapping material classes this object calculates the Diffusion coefficient and 
trapping/detrapping  reaction rates which are required by [Achlys kernels](source/kernels/index.md). [!eqref](eq:diff), 
[!eqref](eq:trap), and [!eqref](eq:detrapp) show  the equations implemented to achieve this.

$D$ is the diffusivity of the species through some material in units of $m^{2}s^{-1}$.
\begin{equation}
\label{eq:diff}
D(T) = D_0 \exp \left( -E_{diff} / k_{B} T \right)
\end{equation}


$\nu_{m}$, in units of $m^{3}s^{-1}$, is the reaction rate for the trapping process and is modelled
by [!eqref](eq:trap) where $\lambda$ is the lattice constant in $m$ and $N_{sol}$ is the
number density of solute sites in the material.
\begin{equation}
\label{eq:trap}
\nu_{m} = \frac{D(T)}{\lambda^2 N_{sol}}
\end{equation}


$\nu_{i}$, in units of $s^{-1}$, is the reaction rate for the de-trapping process from the $i$-th
trapping site. This is modelled by the Arrhenius type equation as given by Equation [!eqref](eq:detrapp)
where E is the energy barrier a trapped atom must overcome to leave the site and $\nu_{0}$ is
referred to as the attempt frequency.
\begin{equation}
\label{eq:detrapp}
\nu_{i} = \nu_0 \exp \left( -E_{i} / k_{B} T \right)
\end{equation}

[comment]: <> (!table id=table:symbol_meanings caption=Symbol meanings)

[comment]: <> (| Parameter Name                     | Symbol&#40;s&#41;     | Unit                 |)

[comment]: <> (|:-----------------------------------|:--------------|:---------------------|)

[comment]: <> (| Species Concentration              | $C, C_i, C_m$ | $m^{-3}$             |)

[comment]: <> (| Trap Concentration                 | $N_i$         | $m^{-3}$             |)

[comment]: <> (| Trapping reaction rate             | $\nu_{m}$     | $m^{3}s^{-1}$        |)

[comment]: <> (| Lattice constant                   | $\lambda$     | $m$                  |)

[comment]: <> (| Lattice sites                      | $N_{sol}$     | $m^{-3}$             |)

[comment]: <> (| De-trapping reaction rate          | $\nu_{i}$     | $s^{-1}$             |)

[comment]: <> (| De-trapping pre-exponential factor | $\nu_0$       | $s^{-1}$             |)

[comment]: <> (| Energy barrier                     | $E$           | eV                   |)

[comment]: <> (| Boltzman Constant                  | $k_b$         | $eV K^{-1}$          |)

[comment]: <> (| Diffusion Coefficient              | $D$           | $m^2\:s^{-1}$        |)

[comment]: <> (| Diffusion pre-exponential factor   | $D_0$         | $m^2\:s^{-1}$        |)

[comment]: <> (| Lattice Density                    | $\rho$        | $m^{-3}$             |)

[comment]: <> (| Temperature                        | $T$           | $K$                  |)

## Trap creation

The rate of extrinsic trap creation in Tungsten irradiated with a particle 
fluence above $1 \times 10^{18} \text{D \:m}^{2}\text{s}^{-1}$ is given by 
[!cite](BONNIN2015) and [!cite](OGOROD2003) as 
shown in equation [!eqref](eq:diff_trap_creation). This accounts for 2 trap-creation processes
each parameterised by a creation rate, $\eta_i$, and a maximum trap density, $n_i$.

\begin{equation}
\frac{dN_{3}}{dt} = (1 - r) \phi \left[ \left(1-\frac{N_3}{n_{3a,max}}\right)\eta_a f(x) + \left(1-\frac{N_3}{n_{3b,max}}\right)\eta_b\theta(x) \right]
\label{eq:diff_trap_creation}
\end{equation}

This is implemented in the ExtrinsicTransientTrappingMaterial class according to
[!eqref](eq:combined_trap_creation)

\begin{equation}
N(x,t) = A\left[ 1-\exp\left( -(1-r)\phi \left[ \frac{\eta_a f(x)}{n_{a,max}} + \frac{\eta_b \theta(x)}{n_{b,max}} \right] t \right) \right]
\label{eq:combined_trap_creation}
\end{equation}

Where the factor, $A$, in [!eqref](eq:combined_trap_creation) is given by the expression in 
[!eqref](eq:a_value)

\begin{equation}
A = \frac{n_{a,max}n_{b,max} \left(f(x)\eta_a + \theta(x)\eta_b \right)}{n_{a,max}\theta(x)\eta_b + n_{b,max}f(x)\eta_a}
\label{eq:a_value}
\end{equation}

!table id=table:symbol_meanings caption=Symbol meanings
| Parameter Name                     | Symbol(s)     | Unit                 |
|:-----------------------------------|:--------------|:---------------------|
| Trap Concentration                 | $N_i$         | $m^{-3}$             |
| Flux                               | $\phi$        | $m^{-2}\:s^{-1}$     |
| Flux reflection fraction           | $r$           | $[]$                 |
| Flux distribution function         | $f(x)$        | $m^{-3} / m^{-2}$    |
| Trap creation rate                 | $\eta_{i}$    | $s^{-1}$             |
| Plastic deformation depth          | $x_p$         | $m$                  |
| Plastic deformation function       | $\theta(x)$     | $m^{-1}$           |



## Example Input File Syntax

!! Describe and include an example of how to use the ExtrinsicTransientTrappingMaterial2 object.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/implant_sub.i block=Variables Materials Functions remove=dts Exponweib_implant

!syntax parameters /Materials/ExtrinsicTransientTrappingMaterial2

!syntax inputs /Materials/ExtrinsicTransientTrappingMaterial2

!syntax children /Materials/ExtrinsicTransientTrappingMaterial2
