import pandas as pd
import matplotlib.pyplot as plt
import math
import numpy as np
from scipy.interpolate import UnivariateSpline as spline

class AnalyticalCalculator:
    def __init__(self, n1 = 0.1, c0 = 1e-4, nsol = 2, E1 = 8.6e-3, Ediff = 0., lambda_val = 3.16e-8,
                 l = 5e-5, T = 1000, tf = 1e-8, v0 = 1e13, D0 = 1.0, D = 0.0, n_times = 101, k = 8.6e-5):
        self.n1 = n1
        self.c0 = c0
        self.nsol = nsol
        self.E1 = E1
        self.Ediff = Ediff
        self.lambda_val = lambda_val
        self.l = l
        self.T = T
        self.tf = tf
        self.v0 = v0
        self.D0 = D0
        self.D = D
        self.k=k
        self.n_times = n_times
        self.times = np.linspace(0,tf,n_times)
        self.D = self._get_D()
        self.trapping_param = self._get_trapping_parameter()
        self.Deff = self._get_D_eff()
    def set(self, **kwargs):
        for key,val in kwargs.items():
            if hasattr(self, key):
                setattr(self, key, val)
        self.times = np.linspace(0., self.tf, self.n_times)
        self.D = self._get_D()
        self.trapping_param = self._get_trapping_parameter()
        self.Deff = self._get_D_eff()
    def _get_trapping_parameter(self):
        val = (self.lambda_val **2) * self.nsol * self.v0 / (self.D0 * self.n1) * np.exp( (self.Ediff - self.E1) / (self.k * self.T) )
        return val
    def _get_D(self):
        D = self.D0 * math.exp(-1 * self.Ediff / (self.k * self.T))
        return D
    def _get_D_eff(self):
        tp = self.trapping_param
        denom = 1.0 + (1.0 / tp)
        Deff = self.D / denom
        # Deff = self.D / (1.0 + (1.0 / self.trapping_param))
        return Deff
    def get_flux_values(self, m_max=100):
        _sum = np.zeros(self.n_times)
        for i in range(m_max):
            m = i + 1
            _sum += (-1)**m * np.exp(-1 * (m ** 2) * (np.pi ** 2) * self.Deff *self.times / (self.l**2))
        flux = np.zeros(self.n_times)
        flux = (self.c0 * self.D / self.l) * (1 + (2 * _sum))
        flux[0] = 0 # maybe hacky. whatever
        return flux


def main():
    a = AnalyticalCalculator()
    flux = a.get_flux_values(100)
    action_data = pd.read_csv('action_out.csv')
    gold_data = pd.read_csv('gold/action_out.csv')
    rho = 3.16e22
    tf = 1e-8
    plt.rc('text', usetex=True)
    fig, axes = plt.subplots(3)
    axes[0].plot(a.times / tf, flux * rho, 'k+', label='Analytical')
    axes[0].plot(gold_data['time']/tf,rho*gold_data['back_flux'], '--',label='gold')
    axes[0].plot(action_data['time']/tf,rho*action_data['back_flux'], '--',label='actions')
    axes[0].set_xlabel(r'$\mathrm{t} / \mathrm{t}_f [\mathrm{-}]$')
    axes[0].set_ylabel(r'$\mathrm{flux\:} [\mathrm{H\: m}^{-2}\mathrm{s}^{-1}]$')

    axes[1].plot(gold_data['time']/tf,np.abs((-gold_data['back_flux'] + action_data['time']) / gold_data['back_flux']), '--',label='gold/actions')
    axes[1].set_xlabel(r'$\mathrm{t} / \mathrm{t}_f [\mathrm{-}]$')
    axes[1].set_ylabel('|Relative fluxes|')
    axes[1].set_yscale('log')

    analytical_spline = spline(a.times / tf, flux * rho, s=0)
    axes[2].plot(gold_data['time']/tf,np.abs((gold_data['back_flux'] -analytical_spline(gold_data['time']))/ analytical_spline(gold_data['time'])), '--',label='gold/analytical')
    axes[2].plot(action_data['time']/tf,np.abs((action_data['back_flux'] -analytical_spline(gold_data['time']))/ analytical_spline(action_data['time'])), '--',label='actions/analytical')
    axes[2].set_xlabel(r'$\mathrm{t} / \mathrm{t}_f [\mathrm{-}]$')
    axes[2].set_ylabel('|Relative fluxes|')
    axes[2].set_yscale('log')
    for ax in axes:
        ax.grid(visible=True,which='major',axis='both')
        ax.minorticks_on()
        ax.grid(visible=True,which='minor',axis='both',alpha=0.2)
        ax.legend()
    plt.show()


if __name__ == "__main__":
    main()

