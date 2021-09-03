---
layout: default
title: Examples
nav_order: 3
---
# One Dimensional Analytical Solution

This example aims to demonstrate a simple set-up to get started with. 

Considering a 1-dimensional domain with a single, homogeneous trap type, a known, constant concentration, $$c_0$$, on one boundary and an initial concentration of $$0$$ on the other boundary. From penalva et al. \cite{PENALVA2012} following \cite{carslaw1959}, the particle flux through second boundary as a function of time can be solved analytically as shown in Equation \ref{eq:1d_analytical} under the condition that $$\zeta >> c_m/n_1$$.

\begin{equation}
\label{eq:1d_analytical}
\varphi(t) = \frac{c_0D}{l} \left[ 1 + 2 \sum_{m=1}^{\infty}(-1)^m\exp\left(-m^2\frac{\pi^2D_{\text{eff}}t}{l^2}\right)\right]
\end{equation} \\
Where $$D_{\text{eff}} $$ is:
\begin{equation}
D_{\text{eff}} = \frac{D}{1 +\zeta}
\end{equation} \\
And $$\zeta$$ is given by:
\begin{equation}
\zeta = \frac{\lambda^2n_{\text{sol}}\nu_0}{D_0n_1}\exp \left(\frac{E_{\text{diff}}- E_1}{k_BT} \right) + \frac{c_m}{n_1}
\end{equation} 

This can be simulated in Blue-kite and compared to the analytical solution. Example input parameters are shown in Table \ref{table:1d_parameters} and simulation set-up will be explained in more detail below. Note that the complete input file is simply the sum of all the individual sections detailed below. 
