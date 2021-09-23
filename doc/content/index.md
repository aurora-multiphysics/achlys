!config navigation breadcrumbs=True scrollspy=True

# Achlys Tritium Diffusion

The Achlys application aims to model macroscopic tritium transport processes through fusion materials 
using the [MOOSE](http://mooseframework.org) Finite Element Framework [!cite](moose_permann2020),  [!cite](moose_multi_gaston2015),  [!cite](moose_ad_lindsay2021).

The Achlys source code is hosted on [Github](https://github.com/aurora-multiphysics/achlys), 
a dockerfile is available to easily obtain the software.

## Governing Equations

Achlys implements a formulation of the Foster-McNabb equations presented by[!cite](HODILLE2015) and 
 [!cite](DELAPORTE2019) as shown in [!eqref](eq:governing_equations_dm), [!eqref](eq:governing_equations_dc), 
and [!eqref](eq:giverning_equations_T).


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

## Kernel selection

The kernels selected for each of the governing equations given in  [!eqref](eq:governing_equations_dm),
 [!eqref](eq:governing_equations_dc), and [!eqref](eq:giverning_equations_T) are shown below. 
Note that all kernels are 
provided by the MOOSE framework without modification except in a single case: 
the ADTrappingEquilibriumEquation is a custom kernel implemented within Achlys.

Note that Automatic Differentiation [!cite](moose_ad_lindsay2021) has been employed within Achlys so 
AD kernels, BCs, and objects should be used wherever available. More information on this system is available through 
the [MOOSE documentation](https://mooseframework.inl.gov/automatic\_differentiation/index.html).

### Mobile Concentration

\begin{equation}
\underbrace{\left(\psi, \frac{\partial C_{m}}{\partial t}\right) }_{\text{(i) \:ADTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, D\nabla C_m \bigg)} _{\text{(ii) \:ADMatDiffusion}}
+ \underbrace{\left(\psi, \sum \frac{\partial C_{t,i}}{\partial t} \right) }_{\text{(iii) \:ADCoupledTimeDerivative}}
+ \underbrace{\bigg( \psi,-S_{ext} \bigg)}_{\text{(iv) \:ADBodyForce}} = 0
\end{equation}

### Trapped Concentration, site $i$

\begin{equation}
\underbrace{\left(\psi, \frac{\partial C_{t,i}}{\partial t}\right) }_{\text{(i) \:ADTimeDerivative}}
- \underbrace{\bigg(\psi, \nu_m C_m(n_i - C_{t,i}) \bigg)
+ \bigg(\psi,\nu_i C_{t,i}  \bigg) }_{\text{( iii ) \: ADTrappingEquilibriumEquation}}
= 0
\end{equation}

### Temperature Evolution

\begin{equation}
\underbrace{\left(\psi, \rho C_p \frac{\partial T}{\partial t}\right) }_{\text{ADHeatConductionTimeDerivative}}
- \underbrace{\bigg(\nabla\psi, k\nabla T \bigg)  }_{\text{ADHeatConduction}} = 0
\end{equation}

## Symbols, units, and meanings

Here, $C$ symbols denote concentrations of the hydrogen isotope either
in the trapped or mobile phase as indicated by the relevant subscript.
$n_i$ is the concentration of a given trap type and other symbols are
defined in [table:symbol_meanings].
Mathematically, the units of concentration can either be expressed in
terms of $m^{-3}$ or atomic fraction; the Achlys application uses
atomic fraction in its calculations.

!table id=table:symbol_meanings caption=Nomenclature and symbol meanings
| Parameter Name                     | Symbol(s)     | Unit                 |
|:-----------------------------------|:--------------|:---------------------|
| Species Concentration              | $C, C_i, C_m$ | $m^{-3}$             |
| Trap Concentration                 | $n_i$         | $m^{-3}$             |
| time                               | $t$           | $s$                  |
| External sources                   | $S_{ext}$     | $m^{-3}s^{-1}$       |
| Trapping reaction rate             | $\nu_{m}$     | $m^{3}s^{-1}$        |
| Lattice constant                   | $\lambda$     | $m$                  |
| Lattice sites                      | $N_{sol}$     | $m^{-3}$             |
| De-trapping reaction rate          | $\nu_{i}$     | $s^{-1}$             |
| De-trapping pre-exponential factor | $\nu_0$       | $s^{-1}$             |
| Energy barrier                     | $E$           | eV                   |
| Boltzman Constant                  | $k_b$         | $eV K^{-1}$          |
| Diffusion Coefficient              | $D$           | $m^2\:s^{-1}$        |
| Diffusion pre-exponential factor   | $D_0$         | $m^2\:s^{-1}$        |
| Lattice Density                    | $\rho$        | $m^{-3}$             |
| Temperature                        | $T$           | $K$                  |
| Thermal Conductivity               | $k$           | $W\:k^{-1}$          |
| Specific Heat Capacity             | $C_p$         | $J \:kg^{-1} K^{-1}$ |
| Mass Density                       | $\rho_m$      | $kg\: m^{-3}$        |

## A Note on Units and Scaling

Achlys solves for the concentrations of the mobile and each trapped phase in the units of atomic fraction,
this is the concentration in $\text{m}^{-3}$ divided by the number density of solute atoms. 
Most physical processes are unaffected by this conversion and the SI units can be retrieved by 
applying a simple scale factor to the outputs, an exception being recombination boundary conditions 
which evaluate the square of the mobile concentration. In this case the scale factor must be re-applied 
within the boundary condition (supplied as a user-input).

 The concentrations are scaled down in this way as a bug was experienced in early development where
 diffusion kernels would cease to function once the magnitude of the scalar field surpassed some threshold.
 This issue is yet to be further investigated. 

 Energies are expressed in eV (though only appear in non-dimensional ratios - so transforms could be possible) 
 and all other units are SI.

# [source](/source)

Complete source definitions are detailed below.

!syntax list /Kernels groups=achlysApp heading=[Achlys kernels](/kernels/index.md) group-headings=False

!syntax list /BCs groups=achlysApp heading=[Achlys BCs](/bcs/index.md) group-headings=False

!syntax list /Materials groups=achlysApp heading=[Achlys Materials](/materials/index.md) group-headings=False

!syntax list /Postprocessors groups=achlysApp heading=[Achlys Postprocessors](/postprocessors/index.md) group-headings=False
