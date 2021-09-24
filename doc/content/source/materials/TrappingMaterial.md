# TrappingMaterial

!alert construction title=Incomplete interface
Currently the trapping material class is hard-coded to require inputs for precisely 3 trap types.
The density and energy parameters can be set to 0 for those not in use. Soon this interface will be replaced 
using array variables to allow more flexibility.


!syntax description /Materials/TrappingMaterial

## Overview

!! Replace these lines with information regarding the TrappingMaterial object.
The trapping material class accepts as inputs all the material parameters expected for basic tritium 
diffusion simulations. Based on inputs provided, the Trapping Material object then declares new material 
properties for all quantities required by the Achlys kernels, namely:

- The diffusion coefficient, $D$
- The trapping reaction rate, $\nu_m$
- The de-trapping reaction rate for each trap, $\nu_i$

These parameters are heavily temperature dependant and will be updated based on a coupled Temperature variable.

$D$ is the diffusivity of the species through some material in units of $m^{2}s^{-1}$.
This is modelled by an Arhhenius relation as given by [!eqref](eq:diff).
The D0 and energy value parameters are determined experimentally for any pair of solute atom and bulk material lattice.
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

## Example Input File Syntax

!listing /test/tests/kernels/one_d_one_trap/one_d_one_trap.i block=Materials

!! Describe and include an example of how to use the TrappingMaterial object.

!syntax parameters /Materials/TrappingMaterial

!syntax inputs /Materials/TrappingMaterial

!syntax children /Materials/TrappingMaterial
