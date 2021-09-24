# ADTrappingEquilibriumEquation

!alert construction title=Undocumented Class
The ADTrappingEquilibriumEquation has not been documented. The content listed below should be used as a starting point for
documenting the class, which includes the typical automatic documentation associated with a
MooseObject; however, what is contained is ultimately determined by what is necessary to make the
documentation clear for users.

!syntax description /Kernels/ADTrappingEquilibriumEquation

## Overview

ADTrappingEquilibriumEquation implements the RHS of [!eqref](eq:governing_equations_dc) which is the
trapping reaction equation governing movement of particles between the trapped and mobile phases. 

\begin{equation}
\frac{\partial C_{t,i}}{\partial t} = \nu_m C_m \left(n_i - C_{t,i} \right) - \nu_iC_{t,i}
\label{eq:governing_equations_dc}
\end{equation}

$C_m$ and $C_t$ represent the local  concentrations of hydrogen isotope in the mobile and trapped phases 
respecitvely. $\nu_m$ and $\nu_i$ are the temperature-dependant reaction rates which are taken from material
properties made available through one the [TrappingMaterial classes](materials/index.md) defined in Achlys.
$n$ is the trap density such that the term in the brackets is zero when all traps are filled.

Residual is accumulated in the trapped concentration and the mobile concentration is the secondary coupled variable.

## Example Input File Syntax

!! Describe and include an example of how to use the ADTrappingEquilibriumEquation object.

!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=Kernels remove=H3_diffusion_eq1 mobile_time_deriv trapped_time_deriv 

!syntax parameters /Kernels/ADTrappingEquilibriumEquation

!syntax inputs /Kernels/ADTrappingEquilibriumEquation

!syntax children /Kernels/ADTrappingEquilibriumEquation
