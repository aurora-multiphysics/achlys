# PfcFunctionFluxRampBC

!alert construction title=Undocumented Class
The PfcFunctionFluxRampBC has not been documented. The content listed below should be used as a starting point for
documenting the class, which includes the typical automatic documentation associated with a
MooseObject; however, what is contained is ultimately determined by what is necessary to make the
documentation clear for users.

!syntax description /BCs/PfcFunctionFluxRampBC

## Overview

!! Replace these lines with information regarding the PfcFunctionFluxRampBC object.

This option implements the integrated boundary condition for the special case of an incident
particle flux with surface recombination as shown in equation [!eqref](eq:bc_incident_with_recomb).


\begin{equation}
\label{eq:bc_incident_with_recomb}
\varphi_{net} = \varphi_{incident}  - K_{r}(T)C_{m}^2
\end{equation}

Input parameters must be supplied for $K_0$ and $E_r$ with a function describing the variation of
the incident flux, $\varphi$, (in time and/or space). Temperature is taken from a coupled variable
and a scale factor must be provided for the conversion of concentration units from $\text{m}^{-3}$
to atomic fraction.

## Example Input File Syntax

!! Describe and include an example of how to use the PfcFunctionFluxRampBC object.

!syntax parameters /BCs/PfcFunctionFluxRampBC

!syntax inputs /BCs/PfcFunctionFluxRampBC

!syntax children /BCs/PfcFunctionFluxRampBC
