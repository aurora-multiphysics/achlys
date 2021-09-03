---
layout: default
title: Thermal Desorption
parent: examples
nav_order: 2
---
# Thermal Desorption Spectrum


This more involved example is a key use-case for the code.

## Extrinsic trap creation

The rate of extrinsic trap creation in Tungsten irradiated with a particle fluence above $$1 \times 10^{18} \text{D m}^{2}\text{s}^{-1}$$ is given by Bonnin et al. \cite{BONNIN2015} and Ogorodnikova et al. \cite{OGOROD2003} as shown in equation \ref{eq:diff_trap_creation}. Here the $a$ and $b$ subscripts relate to the two trap creation processes this model describes and the $\eta$ values represent the creation rates of each process. Other symbols are as defined in Table \ref{table:tpd_parameters}. 
\begin{equation}
\frac{dN_{3}}{dt} = (1 - r) \phi \left[ \left(1-\frac{N_3}{n_{3a,max}}\right)\eta_a f(x) + \left(1-\frac{N_3}{n_{3b,max}}\right)\eta_b\theta(x) \right]
\label{eq:diff_trap_creation}
\end{equation}

Equation \ref{eq:diff_trap_creation} accounts for two trap creation processes, as described by Hodille et al. \cite{HODILLE2015}. The first occurs in the ion implantation region described by the distribution function, $$f(x)$$ . Typically this will give very high rate of trap creation but over a very narrow length (on the nanometer scale). The second gives a lower, uniform distribution over a larger range (on the micrometers scale); this is described by the function $$\theta(x)$$.  

As described in \cite{BONNIN2015}, this accounts for two trap creation processes. The first is due to damage from direct implantation which takes account of the shape of the implantation flux distribution via the factor f(x), this process only occurs in the stopping region. The second is due to material deformation from blistering caused by gas bubbles as the concentration of these gases increases. This has a uniform distribution over the plastic deformation region and is parameterised by the function $$\theta(x)$$ in equation \ref{eq:diff_trap_creation}. 

The flux distribution function, $$f(x)$$, has to be determined by an external tool (such as SRIM) and, from literature \cite{HODILLE2015} \cite{DELAPORTE2019}, $$\theta(x)$$ is taken to be $$1/x_p$$ where $$x_p = 1\mu m$$ in the case we are currently considering. 

For a single trap, the change in number density of the trap as a function of both position and time can be written as equation \ref{eq:single_trap_creation}, as given in e.g. \cite{OGOROD2003} and \cite{BENAN2019}.
\begin{equation}
N(x,t) = n_{max} \left[ 1 - \exp \left( \frac{-(1-r)\phi f(x)\eta t}{n_{max}} \right) \right]
\label{eq:single_trap_creation}
\end{equation}

Similarly, by integration of equation \ref{eq:diff_trap_creation}, the evolution of the combined extrinsic trap density in space and time can be described by equation \ref{eq:combined_trap_creation}.

<div>
\begin{equation}
\begin{split}
N(x,t) &= A\left[ 1-\exp\left( -(1-r)\phi \left[ \frac{\eta_a f(x)}{n_{a,max}} + \frac{\eta_b \theta(x)}{n_{b,max}} \right] t \right) \right] \\
where \\
A &= \frac{n_{a,max}n_{b,max} \left(f(x)\eta_a + \theta(x)\eta_b \right)}{n_{a,max}\theta(x)\eta_b + n_{b,max}f(x)\eta_a}
\end{split}
\label{eq:combined_trap_creation}
\end{equation} 

</div>
The extrinsic trap evolution from Equation \ref{eq:combined_trap_creation} is available in Blue-kite through a derivative of the TrappingMaterial material class called ExtrinsicTransientTrappingMaterial. The resulting extrinsic trap concentrations are shown in Figure \ref{fig:extrinsic_trap_creation} both as a final spatial distribution and the changing concentrations over time at discrete locations. 
