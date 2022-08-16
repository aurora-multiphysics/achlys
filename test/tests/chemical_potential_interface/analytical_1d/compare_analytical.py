import numpy as np
import matplotlib.pyplot as plt
import sys
# MAKE SURE MOOSE_DIR/python is in PYTHONPATH
from vtk.util.numpy_support import vtk_to_numpy
from chigger.exodus import ExodusReader, ExodusSource, ExodusSourceLineSampler



class ExodusResultReader:
    """
    Combining some methods that can get meaningful info out of a moose exodus file
    using chigger and vtk
    """
    def __init__(self, input_file):
        self.reader =  ExodusReader(input_file)
        self.times = self.get_timesteps()
    def get_timesteps(self):
        return self.reader.getTimes()
    def get_actual_values_as_np(self, timestep=0, l=5e-3, array_id = 0, verbose = True, block=None):
        self.reader.setOptions(timestep=timestep)
        source = ExodusSource(self.reader)
        source.update()
        sv = source.getVTKSource()
        sv_o = sv.GetOutput()
        # mobile_data.GetName() == 'Mobile'
        svb = sv_o.GetBlock(0)
        if block != None:
            svb = svb.GetBlock(0)
        pd = svb.GetPointData()
        vtka = pd.GetArray(array_id)
        if verbose:
            print(pd.GetArrayName(array_id))
        np_array = vtk_to_numpy(vtka)
        # assume equispaced cells for now
        n = vtka.GetNumberOfValues()
        x = np.linspace(0,l,n)
        return x, np_array
    def get_values_as_np(self, timestep=0, array_id = 0, verbose = True, block=None):
        self.reader.setOptions(timestep=timestep)
        source = ExodusSource(self.reader)
        source.update()
        sv = source.getVTKSource()
        sv_o = sv.GetOutput()
        # mobile_data.GetName() == 'Mobile'
        svb = sv_o.GetBlock(0)
        if block != None:
            svb = svb.GetBlock(0)
        pd = svb.GetPointData()
        vtka = pd.GetArray(array_id)
        if verbose:
            print(pd.GetArrayName(array_id))
        np_array = vtk_to_numpy(vtka)
        p = svb.GetPoints()
        p_d = p.GetData()
        X = vtk_to_numpy(p_d) # array of [x,y,z] data
        # assume equispaced cells for now
        # n = vtka.GetNumberOfValues()
        # x = np.linspace(0,l,n)
        return X, np_array
    def get_material_values_as_np(self, timestep=0, l=5e-3, array_id = 0, verbose = True):
        """ NOTE: not super robust if values only defined on an edge"""
        self.reader.setOptions(timestep=timestep)
        source = ExodusSource(self.reader)
        source.update()
        sv = source.getVTKSource()
        sv_o = sv.GetOutput()
        # print(mobile_data.GetName())
        svb = sv_o.GetBlock(0)
        cd = svb.GetCellData()
        vtka = cd.GetArray(array_id)
        if verbose:
            print(cd.GetArrayName(array_id))
        np_array = vtk_to_numpy(vtka)
        # assume equispaced cells for now
        n = vtka.GetNumberOfValues()
        x = np.linspace(0,l,n)
        return x, np_array
    def get_values_on_lineAB(self, timestep=0, variable='dcdx',n=20, point_A=[0,0,0], point_B=[0,0,0]):
        self.reader.setOptions(timestep=timestep)
        source = ExodusSource(self.reader)
        ls = ExodusSourceLineSampler(source)
        # point_A = [0.0, 0., 0.]
        # point_B = [0., l, 0.]
        ls.setOptions(point1=point_A)
        ls.setOptions(point2=point_B)
        ls.setOptions(resolution=n)
        ls.update()
        x = ls.getDistance()
        vals = ls.getSample(variable)
        return x, vals


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
    def get_lines(self, n = 100):
        x = np.linspace(0., 1., n)
        y = np.zeros(n)
        p = int(n*self.gamma)
        y[0:p+1] = -0.5 * self.alpha**0.5 * self.f_tild * (x[0:p+1] **2) + \
                 self.a1 * x[0:p+1] + self.b1
        y[p:] = -0.5 * self.alpha**-0.5 * self.f_tild * (x[p:] **2) + \
                self.a2 * x[p:] + self.b2
        return x[0:p],y[0:p], x[p:], y[p:]
    def plot(self):
        # plt.rc('text', usetex=True)
        x,y = self.execute()
        plt.plot(x,y, label='analytical')
        plt.xlabel(r'$\mathrm{x} / \mathrm{L}_f \: [\mathrm{-}]$')
        plt.ylabel(r'$\mathrm{C_m} / \mathrm{C_0}_f \: [\mathrm{-}]$')
        plt.title(r"$\alpha = %1.1f, \beta= %1.1f, \gamma= %1.1f, c_L= %1.1f, f=%1.1f$" % \
                  (self.alpha, self.beta, self.gamma, self.cl, self.f_tild))
        plt.grid(b=True,which='major',axis='both')
        plt.minorticks_on()
        plt.grid(b=True,which='minor',axis='both',alpha=0.2)
        plt.legend()
        plt.show()


def generate_graphs(inputs, filename=None, show=True):
    fig, axes = plt.subplots(len(inputs), sharex=True)
    fig.set_size_inches(8, 6)
    for file, ax in zip(inputs, axes):
        a= AnalyticalIntefaceCalculator(**inputs[file])
        f1 = ExodusResultReader(file)
        X, m1_y = f1.get_values_as_np(timestep = 1, array_id=0, verbose=False)
        _, m2_y = f1.get_values_as_np(timestep = 1, array_id=1, verbose=False)
        x = X[:,0]
        p = int((x.size - 1) * a.gamma)
        x1, y1, x2, y2 = a.get_lines(n = 50)
        ax.plot(x1, y1, 'k+', label='Analytical')
        ax.plot(x[0:p+1], m1_y[0:p+1], 'b-', label='MOOSE')
        ax.plot(x2, y2, 'k+')
        ax.plot(x[p+1:], m2_y[p+1:], 'b-')
        ax.grid(visible=True,which='major',axis='both')
        ax.minorticks_on()
        ax.grid(visible=True,which='minor',axis='both',alpha=0.2)
        ax.set_title(r"$\alpha = %1.1f, \beta= %1.1f, \gamma= %1.1f, c_L= %1.1f, f=%1.1f$" % \
                  (a.alpha, a.beta, a.gamma, a.cl, a.f_tild))
        ax.set_ylabel(r'$\mathrm{C_m} / \mathrm{C_0} \: [\mathrm{-}]$')
        ax.legend()
    axes[1].set_xlabel(r'$\mathrm{x} / \mathrm{L} \: [\mathrm{-}]$')
    fig.suptitle('Analytical chemical potential continuity')
    if filename != None:
        plt.savefig(filename, dpi=300)
    if show:
        plt.show()


if  __name__ == '__main__':
    inputs = {
        'analytical_1d_1_out.e': {},
        'analytical_1d_2_out.e': {
            "alpha": 1.5, "beta": 0.5, "gamma": 0.4, "cl": 0.25, "f_tild": 2.0
        }
    }
    generate_graphs(inputs, filename='analytical.png')
