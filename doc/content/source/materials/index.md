!config navigation breadcrumbs=True scrollspy=True

# Materials

## The Trapping Material Class

New Achlys specific material classes are listed below. These are all based on the concept of the 
trapping material class which is designed to calculate certain material properties required for use
with the [Achlys kernels](source/kernels/index.md), namely:

- The diffusion coefficient, $D$
- The trapping reaction rate, $\nu_m$
- The de-trapping reaction rate for each trap, $\nu_i$

These parameters are heavily temperature dependant and so a coupled Temperature variable is often required for these
material classes. The other input parameters can be seen from the equations below which are required
to calculate values for  $D$, $\nu_m$, and $\nu_i$.

$D$ is the diffusivity of the species through some material in units of $m^{2}s^{-1}$.
This is modelled by an Arhhenius relation as given by [!eqref](eq:diff).

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

## Extrinsic trap creation

For the case where trapping sites are created over time due to particle bombardment some specialised
material classes have been developed. 


- [ExtrinsicTransientTrappingMaterial](source/materials/ExtrinsicTransientTrappingMaterial2.md) for the case of ongoing bombardment and trap creation
- [ExtrinsicStaticTrappingMaterial](source/materials/ExtrinsicStaticTrappingMaterial.md) for constant temperature cases after the creation process has ended
- [ExtrinsicStaticTrappingMaterialRamptingT](source/materials/ExtrinsicStaticTrappingMaterialRampingT.md) for cases with a constant temperature ramp after the creation process has ended, for example in the case of thermal desorption






!syntax list /Materials groups=achlysApp heading=Available Materials
