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

## Symbols, units, and meanings

Here, $$C$$ symbols denote concentrations of the hydrogen isotope either
in the trapped or mobile phase as indicated by the relevant subscript.
$$n_i$$ is the concentration of a given trap type and other symbols are
defined in Table
<a href="#table:symbol_meanings" data-reference-type="ref" data-reference="table:symbol_meanings">1</a>.
Mathematically, the units of concentration can either be expressed in
terms of $$m^{-3}$$ or atomic fraction; the Blue-kite application uses
atomic fraction in its calculations.

<div id="table:symbol_meanings" markdown="1">

| Parameter Name                     | Symbol(s)     | Unit                 |
|:-----------------------------------|:--------------|:---------------------|
| Species Concentration              | $$C, C_i, C_m$$ | $$m^{-3}$$             |
| Trap Concentration                 | $$n_i$$         | $$m^{-3}$$             |
| time                               | $$t$$           | $$s$$                  |
| External sources                   | $$S_{ext}$$     | $$m^{-3}s^{-1}$$       |
| Trapping reaction rate             | $$\nu_{m}$$     | $$m^{3}s^{-1}$$        |
| Lattice constant                   | $$\lambda$$     | $$m$$                  |
| Lattice sites                      | $$N_{sol}$$     | $$m^{-3}$$             |
| De-trapping reaction rate          | $$\nu_{i}$$     | $$s^{-1}$$             |
| De-trapping pre-exponential factor | $$\nu_0$$       | $$s^{-1}$$             |
| Energy barrier                     | $$E$$           | eV                   |
| Boltzman Constant                  | $$k_b$$         | $$eV K^{-1}$$          |
| Diffusion Coefficient              | $$D$$           | $$m^2\:s^{-1}$$        |
| Diffusion pre-exponential factor   | $$D_0$$         | $$m^2\:s^{-1}$$        |
| Lattice Density                    | $$\rho$$        | $$m^{-3}$$             |
| Temperature                        | $$T$$           | $$K$$                  |
| Thermal Conductivity               | $$k$$           | $$W\:k^{-1}$$          |
| Specific Heat Capacity             | $$C_p$$         | $$J \:kg^{-1} K^{-1}$$ |
| Mass Density                       | $$\rho_m$$      | $$kg\: m^{-3}$$        |

Nomenclature and symbol meanings

</div>

<span id="table:symbol_meanings"
label="table:symbol_meanings">\[table:symbol_meanings\]</span>

$$D$$ is the diffusivity of the species through some material in units of
$$m^{2}s^{-1}$$. This is modelled by an Arhhenius relation as given by
Equation
<a href="#eq:diff" data-reference-type="ref" data-reference="eq:diff">[eq:diff]</a>.
The D0 and energy value parameters are determined experimentally for any
pair of solute atom and bulk material lattice. \begin{equation}\label{eq:diff}
D(T) = D_0 \exp \left( -E_{diff} / k_{B} T \right)\end{equation}

$$\nu_{m}$$, in units of $$m^{3}s^{-1}$$, is the reaction rate for the
trapping process and is modelled by Equation
<a href="#eq:trap" data-reference-type="ref" data-reference="eq:trap">[eq:trap]</a>
where $$\lambda$$ is the lattice constant in $$m$$ and $$N_{sol}$$ is the
number density of solute sites in the material. \begin{equation}\label{eq:trap}
\nu_{m} = \frac{D(T)}{\lambda^2 N_{sol}}\end{equation}

$$\nu_{i}$$, in units of $$s^{-1}$$, is the reaction rate for the
de-trapping process from the $$i$$-th trapping site. This is modelled by
the Arrhenius type equation as given by Equation
<a href="#eq:detrapp" data-reference-type="ref" data-reference="eq:detrapp">[eq:detrapp]</a>
where E is the energy barrier a trapped atom must overcome to leave the
site and $$\nu_{0}$$ is referred to as the attempt frequency.
\begin{equation}\label{eq:detrapp}
\nu_{i} = \nu_0 \exp \left( -E_{i} / k_{B} T \right)\end{equation}

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


