# DirichletEquivalentPfcImplantBC

!alert construction title=Undocumented Class
The DirichletEquivalentPfcImplantBC has not been documented. The content listed below should be used as a starting point for
documenting the class, which includes the typical automatic documentation associated with a
MooseObject; however, what is contained is ultimately determined by what is necessary to make the
documentation clear for users.

!syntax description /BCs/DirichletEquivalentPfcImplantBC

## Overview

!! Replace these lines with information regarding the DirichletEquivalentPfcImplantBC object.

Implements a dirichlet boundary condition where the value at the surface is calculated
as the steady-state balance between a known incident flux of particles into the material
and an opposing desorption flux back out of the material, which is
based on a user-defined recombination condition. 

This option is not necessarily intended to offer a good general physical representation but
is used to replicate the numerical model of [!cite](DELAPORTE2019). 


This boundary condition aims to simplify 
[!eqref](eq:bc_incident_with_recomb) for the particular case of steady-state equilibrium, 
as shown in [!eqref](eq:bc_dirichlet_equiv).


\begin{equation}
\label{eq:bc_incident_with_recomb}
\varphi_{net} = \varphi_{incident}  - K_{r}(T)C_{m}^2
\end{equation}

Where the recombination coefficient, $K_r$, is given by [!eqref](eq:recombination_coef).

\begin{equation}
\label{eq:recombination_coef}
K_{r}(T) =  K_0\exp\left(-E_r / k_B T\right)
\end{equation}

At equilibirum, the recombination flux is equal to the incident flux and the mobile concentration can be
found from [!eqref](eq:bc_dirichlet_equiv).

\begin{equation}
\label{eq:bc_dirichlet_equiv}
C_m = \sqrt{\frac{\varphi}{K_r(T)}}
\end{equation}

User inputs are required for $K_0$, $E_r$, and $\varphi$. 
The temperature is taken from a coupled variable. 
Note that again a scale factor is required for the conversion of the mobile concentration from $\text{m}^{-3}$ 
to atomic fraction.

## Example Input File Syntax

!! Describe and include an example of how to use the DirichletEquivalentPfcImplantBC object.

!syntax parameters /BCs/DirichletEquivalentPfcImplantBC

!syntax inputs /BCs/DirichletEquivalentPfcImplantBC

!syntax children /BCs/DirichletEquivalentPfcImplantBC
