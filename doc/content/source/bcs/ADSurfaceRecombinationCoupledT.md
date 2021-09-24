# ADSurfaceRecombinationCoupledT

!alert construction title=Undocumented Class
The ADSurfaceRecombinationCoupledT has not been documented. The content listed below should be used as a starting point for
documenting the class, which includes the typical automatic documentation associated with a
MooseObject; however, what is contained is ultimately determined by what is necessary to make the
documentation clear for users.

!syntax description /BCs/ADSurfaceRecombinationCoupledT

## Overview

!! Replace these lines with information regarding the ADSurfaceRecombinationCoupledT object.

The Surface Recombination boundary condition implements an outflow proprotional to the square of the local
mobile species concentration as given in [!eqref](eq:bc_recombination). 

\begin{equation}
\label{eq:bc_recombination}
\varphi_{out} =  K_{r}(T)C_{m}^2
\end{equation}

Here the recombination coefficient, $k_r$ is parameterised according to an Arrhenious expression as 
given in [!eqref](eq:recombination_coef).

\begin{equation}
\label{eq:recombination_coef}
K_{r}(T) =  K_0\exp\left(-E_r / k_B T\right)
\end{equation}

User input is required for the parameters of the recombination coefficient. 
Temperature should be provided as a coupled variable. Note an equivalent constant-temperature bc is 
available through [ADSurfaceRecombination](/bcs/ADSurfaceRecombination.md).

## Example Input File Syntax

!! !listing 

!! Describe and include an example of how to use the ADSurfaceRecombinationCoupledT object.

!syntax parameters /BCs/ADSurfaceRecombinationCoupledT

!syntax inputs /BCs/ADSurfaceRecombinationCoupledT

!syntax children /BCs/ADSurfaceRecombinationCoupledT
