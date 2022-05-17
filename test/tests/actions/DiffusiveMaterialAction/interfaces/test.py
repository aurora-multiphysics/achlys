import pandas as pd
import math
import numpy as np

class AnalyticalIntefaceCalculator:
    def __init__(self, alpha=2., beta=1.5, gamma=0.6, cl=2., f_tild=1.):
        self.alpha = alpha
        self.beta = beta
        self.gamma = gamma
        self.cl = cl
        self.f_tild = f_tild
        self.a0=0.
        self.a1=0.
        self.a2=0.
        self.b1 = 1.
        self.b2=0.
        self.update_coeffs()
    def set(self, **kwargs):
        for key,val in kwargs.items():
            if hasattr(self, key):
                setattr(self, key,val)
        self.update_coeffs()
    def update_coeffs(self):
        self.a0 = self._calc_a0()
        self.a1 = self._calc_a1()
        self.a2 = self._calc_a2()
        self.b2 = self._calc_b2()
    def _calc_a0(self):
        # nom = (2 * self.alpha**0.5 * (self.cl - self.beta)) + \
        #       (self.f_tild * (self.gamma**2 * (1. - self.alpha * self.beta) - 1.))
        nom = (2 * self.alpha**0.5 * (self.cl - self.beta)) + \
              (self.f_tild * (self.gamma**2 * (self.alpha * self.beta - 1.) + 1.))
        denom = 2 * (1. + (self.alpha * self.beta *self.gamma) - self.gamma)
        return nom / denom
    def _calc_a1(self):
        return self.a0 * self.alpha **0.5
    def _calc_a2(self):
        return self.a0 * self.alpha **-0.5
    def _calc_b2(self):
        return self.cl + 0.5*self.alpha**-0.5 * self.f_tild - self.a2
    def execute(self, n = 100):
        x = np.linspace(0., 1., n)
        y = np.zeros(n)
        p = int(n*self.gamma)
        y[0:p] = -0.5 * self.alpha**0.5 * self.f_tild * (x[0:p] **2) + \
                 self.a1 * x[0:p] + self.b1
        y[p:] = -0.5 * self.alpha**-0.5 * self.f_tild * (x[p:] **2) + \
                self.a2 * x[p:] + self.b2
        return x,y


