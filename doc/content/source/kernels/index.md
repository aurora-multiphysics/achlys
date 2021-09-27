!config navigation breadcrumbs=True scrollspy=True

# Kernels

The list of Achlys specific kernel implementations are given [here](#kernel_list). 
The following sections give the full description of the kernel selections for implementnig 
 the governing equations shown in  [!eqref](eq:governing_equations_dm), [!eqref](eq:governing_equations_dc),
and [!eqref](eq:giverning_equations_T). The descriptions include
links to the MOOSE framework documentation where standard kernels can be employed.

\begin{equation}
\frac{\partial C_{m}}{\partial t} = \nabla  \cdot \left( D \left(T \right) \nabla  C_{m} \right) - \sum \frac{\partial C_{t,i}}{\partial t} + S_{ext}
\label{eq:governing_equations_dm}
\end{equation}

\begin{equation}
\frac{\partial C_{t,i}}{\partial t} = \nu_m \left(T\right) C_m \left(n_i - C_{t,i} \right) - \nu_i\left(T\right) C_{t,i}
\label{eq:governing_equations_dc}
\end{equation}

\begin{equation}
\label{eq:giverning_equations_T}
\rho_m C_p \frac{\partial T}{\partial t} = \nabla \cdot \left(k \nabla T \right)
\end{equation}

Note that some Achlys kernels are designed for use with specific 
[achlys materials](source/materials/index.md) such as the Trapping
Material class which calculates the diffusion coefficient and 
trapping/detrapping reaction rates.

## Mobile Concentration

The first of the governing equations gives the mass balance for
the mobile phase of dissolved atoms. This is a balance between the source/sink
terms, given by either external sources or the coupling to the trapping/detrapping
process, and diffusive transport driven by a concentration gradient.

Note that the coupled time derivative here provides the coupling 
between the mobile and trapped phase mass balances.

\begin{equation}
\underbrace{\left(\psi, \frac{\partial C_{m}}{\partial t}\right) }_{\text{(i) \:ADTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, D\nabla C_m \bigg)} _{\text{(ii) \:ADMatDiffusion}}
+ \underbrace{\left(\psi, \sum \frac{\partial C_{t,i}}{\partial t} \right) }_{\text{(iii) \:ADCoupledTimeDerivative}}
+ \underbrace{\bigg( \psi,-S_{ext} \bigg)}_{\text{(iv) \:ADBodyForce}} = 0
\end{equation}

| Number| Description| Framework / Achlys | Link|
|:-|:-|:-|:-|
| (i) | Time derivative | Framework | [ADTimeDerivative](https://mooseframework.inl.gov/source/kernels/ADTimeDerivative.html) |
| (ii) | Diffusion | Framework | [ADMatDiffusion](https://mooseframework.inl.gov/source/kernels/ADMatDiffusion.html) |
| (iii) | Coupled time derivate | Framework | [ADCoupledTimeDerivative](https://mooseframework.inl.gov/source/kernels/ADCoupledTimeDerivative.html) |
| (iv) | Source term | Framework | [ADBodyForce](https://mooseframework.inl.gov/source/kernels/BodyForce.html) |




## Trapped Concentration, site $i$

The second of the governing equations must be implemented once for each
material trapping site being considered. This is achieved using the following
kernels:

\begin{equation}
\underbrace{\left(\psi, \frac{\partial C_{t,i}}{\partial t}\right) }_{\text{(i) \:ADTimeDerivative}}
- \underbrace{\bigg(\psi, \nu_m C_m(n_i - C_{t,i}) \bigg)
+ \bigg(\psi,\nu_i C_{t,i}  \bigg) }_{\text{( iii ) \: ADTrappingEquilibriumEquation}}
= 0
\end{equation}


| Number| Description| Framework / Achlys | Link|
|:-|:-|:-|:-|
| (i) | Time derivative | Framework | [ADTimeDerivative](https://mooseframework.inl.gov/source/kernels/ADTimeDerivative.html) |
| (ii) | Trapping equilibrium | Achlys | [ADEquilibriumEquation](ADTrappingEquilibriumEquation.md) |


## Temperature Evolution

Thermal transport calculations always use the [MOOSE Heat Conduction App](https://mooseframework.inl.gov/modules/heat_conduction/index.html)
without any modification or enhancement.

\begin{equation}
\underbrace{\left(\psi, \rho C_p \frac{\partial T}{\partial t}\right) }_{\text{ADHeatConductionTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, k\nabla T \bigg)  }_{\text{ADHeatConduction}} = 0
\end{equation}


| Number| Description| Framework / Achlys | Link|
|:-|:-|:-|:-|
| (i) | Time derivative | Heat Conduction Module | [ADHeatConductionTimeDerivative](https://mooseframework.inl.gov/source/kernels/ADHeatConductionTimeDerivative.html) |
| (ii) | Heat conduction (Diffusion) | Heat Conduction Module | [ADHeatConduction](https://mooseframework.inl.gov/source/kernels/ADHeatConduction.html) |

## Example usage

Drawing together the previous sections, the following example shows
how to construct the kernels input block for an (isothermal) achlys calculation 
with 3 material traps.


!listing /problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i block=Variables Kernels

!syntax list /Kernels groups=achlysApp heading=Available Kernels id=kernel_list
