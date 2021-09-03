---
layout: default
title: Introduction
nav_order: 2
---

$$\require{asmath}$$
# Achlys
The Achlys application aims to model macroscopic tritium transport processes through fusion materials using the MOOSE Finite Element Framework { moose_permann2020 } \cite{moose_multi_gaston2015} \cite{moose_ad_lindsay2021}. Achlys implements a formulation of the Foster-McNabb equations presented by Hodille et al. \cite{HODILLE2015} and Delaporte et al. \cite{DELAPORTE2019} Benchmarking of blue-kite is currently being carried out against published computational and experimental results.

# Governing Equations
As detailed in Delaporte et al. \cite{DELAPORTE2019} following Hodille et al. \cite{HODILLE2015}, the system of governing diffusion-reaction equations with coupled transient heat conduction can be expressed as shown in Equations \ref{eq:governing_equations_dm} - \ref{eq:giverning_equations_T}. Note that Equations  \ref{eq:governing_equations_dm}  and \ref{eq:governing_equations_dc} are completely coupled, whereas the temperature dependence only acts one way. Temperature evolution itself can be evaluated entirely independently but the transport processes are highly temperature dependent.  
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

# Kernel Selection
The selection of each kernel is laid out in more detail in the sections below, but the results are presented here for a quick reference. Note that all kernels are provided by the MOOSE framework without modification except in a single case: the TrappingEquilibriumEquation is a custom kernel implemented within Achlys. 

### Mobile Concentration
<div style="text-align: left">
\begin{equation}
\underbrace{\left(\psi, \frac{\partial C_{m}}{\partial t}\right) }_{\text{(i) ADTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, D\nabla C_m \bigg)} _{\text{(ii) ADMatDiffusion}}
+ \underbrace{\left(\psi, \sum \frac{\partial C_{t,i}}{\partial t} \right) }_{\text{(iii) ADCoupledTimeDerivative}}
+ \underbrace{\bigg( \psi,-S_{ext} \bigg)}_{\text{(iv) ADBodyForce}} = 0
\end{equation}
</div>

### Trapped Concentration 
<div>
\begin{equation}
\underbrace{\left(\psi, \frac{\partial C_{t,i}}{\partial t}\right) }_{\text{(i) ADTimeDerivative}}
- \underbrace{\bigg(\psi, \nu_m C_m(n_i - C_{t,i}) \bigg) 
+ \bigg(\psi,\nu_i C_{t,i}  \bigg) }_{\text{( ii )  ADTrappingEquilibriumEquation}}
= 0
\end{equation}</div> 

### Temperature Evolution
<div>
\begin{equation}
\underbrace{\left(\psi, \rho C_p \frac{\partial T}{\partial t}\right) }_{\text{ADHeatConductionTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, k\nabla T \bigg)  }_{\text{ADHeatConduction}} = 0
\end{equation}
</div>


