import numpy as np
import matplotlib.pyplot as plt
import sys
sys.path.append("/Users/sdixon/projects/moose/python")
# import chigger
from vtk.util.numpy_support import vtk_to_numpy
from chigger.exodus import ExodusReader, ExodusSource, ExodusSourceLineSampler

class ExactSolution:
    def __init__(self, s1=1., e1=0.1, s2 =2., e2=0.2, correction=False):
        self.L = 1.
        self.H = 1.
        self.s1 = s1
        self.s2 = s2
        self.e1 = e1
        self.e2 = e2
        self.correction = correction
    def _T(self, x, y, t):
        return 500 + (30 * np.cos(2*np.pi*x) * np.cos(2*np.pi*y) * np.cos(2*np.pi*t) )
    def _cm1(self, x, y , t):
        return 2 + ( np.cos(2*np.pi*x) * np.cos(2*np.pi*y)) + t
    def _Rs(self, x, y, t):
        T = self._T(x,y,t)
        # return self.s2 / self.s1 * np.exp( (self.e1 - self.e2) / (8.617e-5 * T))
        s1 =  self.s1 * np.exp( -(self.e1 ) / (8.617e-5 * T))
        s2 = self.s2 * np.exp( -(self.e2) / (8.617e-5 * T))
        return s2 / s1
    def _cm2(self,x ,y, t):
        cm1 = self._cm1(x,y,t)
        Rs = self._Rs(x,y,t)
        return  (1 + Rs) * cm1 if self.correction else Rs * cm1
    def combine_results(self, t, n=[51, 51]):
        nx =  n[0]
        ny = n[1]
        x = np.linspace(0,self.L, nx)
        y = np.linspace(0, self.H, ny)
        X, Y = np.meshgrid(x, y, indexing='ij')
        cm = np.zeros(n)
        p = nx // 2
        cm[0:p,:] = self._cm1(X[0:p,:],Y[0:p,:],t)
        cm[p:,:] = self._cm2(X[p:,:],Y[p:,:],t)
        return X, Y, cm
    def plot(self, t,  n = 100):
        x = np.linspace(0,self.L, n)
        y = np.linspace(0, self.H, n)
        X, Y = np.meshgrid(x, y, indexing='ij')
        cm = np.zeros([n,n])
        p = n // 2
        cm[0:p,:] = self._cm1(X[0:p,:],Y[0:p,:],t)
        cm[p:,:] = self._cm2(X[p:,:],Y[p:,:],t)
        plt.contour(X,Y,cm, colors='w', linewidths=0.7)
        plt.contourf(X,Y,cm, levels=100)
        plt.colorbar()
        # plt.plot(0.2, 0.5, 'kx')
        # plt.plot(0.1, 0.7, 'kx')
        # print(cm[10,70])
        plt.show()
    def plot_T(self, t,  n = 100):
        x = np.linspace(0,self.L, n)
        y = np.linspace(0, self.H, n)
        X, Y = np.meshgrid(x, y, indexing='ij')
        T = self._T(X,Y,t)
        plt.contourf(X,Y,T)
        plt.colorbar()
        # plt.plot(0.7, 0.1, 'kx')
        # print(T[70,10])
        # print(self._T(0.7,0.1,t))
        plt.show()
    def plot_Rs(self, t,  n = 100):
        x = np.linspace(0,self.L, n)
        y = np.linspace(0, self.H, n)
        X, Y = np.meshgrid(x, y, indexing='ij')
        Rs = self._Rs(X,Y,t)
        plt.contourf(X,Y,Rs)
        plt.colorbar()
        # plt.plot(0.7, 0.1, 'kx')
        # print(Rs[70,10])
        # print(self._Rs(0.7,0.1,t))
        plt.show()
    def plot_cm1(self, t,  n = 100):
        x = np.linspace(0,self.L, n)
        y = np.linspace(0, self.H, n)
        X, Y = np.meshgrid(x, y, indexing='ij')
        cm = self._cm1(X,Y,t)
        plt.contourf(X,Y,cm)
        plt.colorbar()
        plt.show()


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


class MooseResultProcessor:
    def __init__(self, file):
        self.data = ExodusResultReader(file)
    def get_timestep(self, timestep):
        X, cm1 = self.data.get_values_as_np(timestep=timestep, verbose=False)
        _, cm2 = self.data.get_values_as_np(timestep=timestep, array_id = 1, verbose=False)
        # X is an array of (x,y,z) co-ordinates of the result vector (cm1 or cm2 here)
        return X,cm1,cm2
    def combine_results(self, timestep):
        X,cm1,cm2 = self.get_timestep(timestep)
        # n = int(np.sqrt(X.shape[0]))
        nx = len(set(X[:,0]))
        ny = len(set(X[:,1]))
        Z = np.zeros([nx,ny])
        xi = np.linspace(0,1,nx)
        yi = np.linspace(0,1,ny)
        xx, yy =np.meshgrid(xi,yi, indexing='ij')
        ex = 1. / (nx * 100)
        ey = 1. / (ny * 100)
        for i,x in enumerate(X):
            x_index = np.where(np.abs(xi - x[0]) < ex)[0][0]
            y_index = np.where(np.abs(yi - x[1]) < ey)[0][0]
            if x_index < nx //2:
                Z[x_index, y_index] = cm1[i]
            else:
                Z[x_index, y_index] = cm2[i]
        return xx,yy,Z
    def plot(self, timestep, dt=5e-4, show=True, filename=None):
        x,y,z = self.combine_results(timestep)
        plt.rc('text', usetex=True)
        plt.contourf(x,y,z, levels=100)
        clb = plt.colorbar()
        clb.ax.set_title('C')
        plt.contour(x,y,z,colors='w', linewidths=0.7)
        fontsize = 17
        plt.ylabel(r'$\mathrm{y\: }\left(\mathrm{m}\right)$', fontsize = fontsize)
        plt.xlabel(r'$\mathrm{x \:} \left(\mathrm{m} \right)$', fontsize = fontsize)
        plt.title(r't = %s s' % (timestep * dt))
        if filename != None:
            plt.savefig(filename,dpi=300)
        if show:
            plt.show()


def compare(file, time=0.01, dt=5e-4, err_type='frac', lines=True, plot=True, filename=None):
    # file = 'chempot_mms_out.e'
    # dt = 5e-4
    timestep = int(time / dt)
    a = MooseResultProcessor(file)
    b = ExactSolution()
    X,Y,Za = a.combine_results(timestep=timestep)
    n = X.shape
    X2, Y2, Zb = b.combine_results(t=time, n=n)
    if err_type == 'frac':
        frac = np.abs(Zb-Za)/Zb * 100
    else:
        frac = np.abs(Zb - Za)
    x_diff = X2-X
    y_diff = Y2-Y
    if x_diff.sum() != 0:
        print("x_diff test: %f" % x_diff.sum())
    if y_diff.sum() != 0:
        print("xydiff test: %f" % y_diff.sum())
    plt.contourf(X,Y,frac, levels=100)
    plt.colorbar()
    if lines:
        plt.contour(X,Y,frac,colors='w', linewidths=0.7)
    if err_type=='frac':
        plt.title('Percent Error')
    else:
        plt.title('Abs Error')
    if filename != None:
        plt.savefig(filename,dpi=300)
    if plot:
        plt.show()


def generate_plots(times, dt=2e-3, filename=None, show=True):
    fig, axes = plt.subplots(ncols=2, nrows=len(times))
    fig.set_size_inches(12, 8)
    for i, time in enumerate(times):
        timestep = int(time / dt)
        a = MooseResultProcessor(file)
        b = ExactSolution()
        X,Y,Za = a.combine_results(timestep=timestep)
        n = X.shape
        X2, Y2, Zb = b.combine_results(t=time, n=n)
        frac = np.abs(Zb-Za)/Zb * 100
        ax1 = axes[i, 1]
        plot1 = ax1.contourf(X,Y,frac, levels=100)
        plt.colorbar(plot1, ax=ax1)
        ax1.set_title('Percent error at t=%1.2f s' % time)
        ax2 = axes[i, 0]
        ax2.contour(X,Y,Za, colors='w', linewidths=0.7)
        plot2 = ax2.contourf(X,Y,Za, levels=100)
        plt.colorbar(plot2, ax=ax2)
        ax2.set_title('Solution at t=%1.2f s' % time)
    fig.suptitle('2D MMS study for chemical potential continuity with dt=%1.0e and n=%d' % (dt, np.prod(n)))
    if filename != None:
        plt.savefig(filename,dpi=300)
    if show:
        plt.show()


if __name__ == '__main__':
    file = 'mms_out.e'
    times = [0.01, 0.04]
    generate_plots(times, filename='mms.png', dt=5e-3)

