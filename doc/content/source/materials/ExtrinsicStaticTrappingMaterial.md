# ExtrinsicStaticTrappingMaterial

!syntax description /Materials/ExtrinsicStaticTrappingMaterial

## Overview

This class implements the trapping material class for the case where additional extrinsic trapping sites
have previously been created due to the influence of heavy particle bombardment. 


Note the [transient class](source/materials/ExtrinsicTransientTrappingMaterial.md) exists for cases
where traps conitnue to be created over time. This class aims only to maintain the final state after
some period of particle implantation is complete and the precise distribution of new traps is required for
subsequent transport calculations.

Note there are a number of complimentary classes for use in cases where extrinsic trap creation is taking place or has taken place:

- [ExtrinsicTransientTrappingMaterial](source/materials/ExtrinsicTransientTrappingMaterial2.md) for the case of ongoing bombardment and trap creation
- [ExtrinsicStaticTrappingMaterial](source/materials/ExtrinsicStaticTrappingMaterial.md) for constant temperature cases after the creation process has ended
- [ExtrinsicStaticTrappingMaterialRamptingT](source/materials/ExtrinsicStaticTrappingMaterialRampingT.md) for cases with a constant temperature ramp after the creation process has ended, for example in the case of thermal desorption


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

This is implemented in the Extrinsic Trapping Material classes according to
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

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/resting_multi.i  block=Variables Materials Functions remove=dts Exponweib_implant

!! Describe and include an example of how to use the ExtrinsicStaticTrappingMaterial object.

!syntax parameters /Materials/ExtrinsicStaticTrappingMaterial

!syntax inputs /Materials/ExtrinsicStaticTrappingMaterial

!syntax children /Materials/ExtrinsicStaticTrappingMaterial
