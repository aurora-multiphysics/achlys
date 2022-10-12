import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

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


def steady_state_plot(data_file, filename=None, array_id=0, linespec='-', show=True, interface='chem', title=""):
    plt.rc('text', usetex=True)
    fontsize = 17
    a = ExodusResultReader(data_file)
    X, y = a.get_values_as_np(timestep=1, array_id=array_id, block=None, verbose=False)
    if interface == "D":
        label = r'continuity of $C_m$'
    else:
        label = r'continuity of $\mu$'
    plt.plot(X[:,0],y, linespec, label = label)
    plt.ylabel(r'$\mathrm{Solute\: concentration\: }\left(\mathrm{D}\: \mathrm{m}^{-3}\right)$', fontsize = fontsize)
    plt.xlabel(r'$\mathrm{Depth \:} \left(\mathrm{m} \right)$', fontsize = fontsize)
    plt.grid(visible=True,which='major',axis='both')
    plt.minorticks_on()
    plt.grid(visible=True,which='minor',axis='both',alpha=0.2)
    plt.legend()
    plt.title(title)
    if filename != None:
        plt.savefig(filename,dpi=300)
    if show:
        plt.show()


if __name__ == "__main__":
    steady_state_plot('fluxcont_WCu_out.e', array_id=1, interface='D', show=False)
    steady_state_plot('chempot_WCu_out.e', array_id=1,  linespec='--', filename='WCu_steady_state.png', title="W CU")
    steady_state_plot('fluxcont_CuEurofer_out.e', array_id=1, interface='D', show=False)
    steady_state_plot('chempot_CuEurofer_out.e', array_id=1,  linespec='--', filename='CuEurofer_steady_state.png', title="Cu Eurofer")

