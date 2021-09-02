---
layout: default
title: Introduction
nav_order: 2
---

# Achlys
The Blue-Kite application aims to model macroscopic tritium transport processes through fusion materials using the MOOSE Finite Element Framework { moose_permann2020 } \cite{moose_multi_gaston2015} \cite{moose_ad_lindsay2021}. It sits amongst a small suite of similar codes (Red-kite most notably) differing in the precise scheme of equations it aims to solve; blue-kite is also distinct in implementing its own kernel, material, BC, and post-processor code instead of relying solely on the bare framework or standard modules distributed with MOOSE. Blue kite specifically implements a formulation of the Foster-McNabb equations presented by Hodille et al. \cite{HODILLE2015} and Delaporte et al. \cite{DELAPORTE2019} Benchmarking of blue-kite is currently being carried out against published computational and experimental results.

This document aims to present (i) the physical basis of the calculations being performed, (ii) the rationale underlying development decisions, and (iii) sufficient information on the set-up and execution of simulations through both discussion and example. Production releases of the code will be distributed as containerised images so installation will not be discussed here.  
\ref{eq:governing_equations_dm} \ref{eq:governing_equations_dc}

# Governing Equations
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
The selection of each kernel is laid out in more detail in the sections below, but the results are presented here for a quick reference. Note that all kernels are provided by the MOOSE framework without modification except in a single case: the TrappingEquilibriumEquation is a custom kernel implemented within Blue-Kite. 

<div>
\begin{equation}
\tag{Mobile Concentration}
\underbrace{\left(\psi, \frac{\partial C_{m}}{\partial t}\right) }_{\text{\emph{(i)} \:ADTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, D\nabla C_m \bigg)} _{\text{\emph{(ii)} \:ADMatDiffusion}}
+ \underbrace{\left(\psi, \sum \frac{\partial C_{t,i}}{\partial t} \right) }_{\text{\emph{(iii)} \:ADCoupledTimeDerivative}}
+ \underbrace{\bigg( \psi,-S_{ext} \bigg)}_{\text{\emph{(iv)} \:ADBodyForce}} = 0
\end{equation}
</div>

<div>
\begin{equation}
\tag{Trapped Concentration, site $i$}
\underbrace{\left(\psi, \frac{\partial C_{t,i}}{\partial t}\right) }_{\text{(\emph{i}) \:ADTimeDerivative}}
- \underbrace{\bigg(\psi, \nu_m C_m(n_i - C_{t,i}) \bigg) 
+ \bigg(\psi,\nu_i C_{t,i}  \bigg) }_{\text{( \emph{iii} ) \: ADTrappingEquilibriumEquation}}
= 0
\end{equation}</div> 

<div>
\begin{equation}
\tag{Temperature Evolution}
\underbrace{\left(\psi, \rho C_p \frac{\partial T}{\partial t}\right) }_{\text{ADHeatConductionTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, k\nabla T \bigg)  }_{\text{ADHeatConduction}} = 0
\end{equation}</div>

