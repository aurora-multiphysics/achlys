# ADSurfaceRecombination

!alert construction title=Undocumented Class
The ADSurfaceRecombination has not been documented. The content listed below should be used as a starting point for
documenting the class, which includes the typical automatic documentation associated with a
MooseObject; however, what is contained is ultimately determined by what is necessary to make the
documentation clear for users.

!syntax description /BCs/ADSurfaceRecombination

## Overview

!! Replace these lines with information regarding the ADSurfaceRecombination object.

The Surface Recombination boundary condition implements an outflow propotional to the square of the local
mobile species concentration as given in [!eqref](eq:bc_recombination). This particular version only considers the specific case where the temperature is a single, constant value 
across the boundary it is applied to.

\begin{equation}
\label{eq:bc_recombination}
\varphi_{out} =  K_{r}C_{m}^2
\end{equation}

Here the recombination coefficient, $k_r$ is parameterised according to an Arrhenious expression as
given in [!eqref](eq:recombination_coef).

\begin{equation}
\label{eq:recombination_coef}
K_{r}(T) =  K_0\exp\left(-E_r / k_B T\right)
\end{equation}

User input is required for the parameters of the recombination coefficient and a value for the 
constant temperature to be considered.

For cases with non-constant tempreature see
[ADSurfaceRecombinationCoupledT](/bcs/ADSurfaceRecombinationCoupledT.md) instead.

## Example Input File Syntax

!! Describe and include an example of how to use the ADSurfaceRecombination object.

!syntax parameters /BCs/ADSurfaceRecombination

!syntax inputs /BCs/ADSurfaceRecombination

!syntax children /BCs/ADSurfaceRecombination
