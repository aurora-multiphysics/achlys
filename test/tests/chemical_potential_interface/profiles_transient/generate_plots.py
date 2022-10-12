import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def transient_plot(csv_file, filename=None, linespec='-', show=True, interface='chem', title=""):
    plt.rc('text', usetex=True)
    fontsize = 17
    data = pd.read_csv(csv_file)
    if interface == "D":
        label = r'continuity of $C_m$'
    else:
        label = r'continuity of $\mu$'
    plt.plot(data['time'], data['flux'], linespec, label = label)
    plt.ylabel(r'$\mathrm{Outgassing\: flux\: }\left(\mathrm{D}\: \mathrm{m}^{-2}\: \mathrm{s}^{-1}\right)$', fontsize = fontsize)
    plt.xlabel(r'$\mathrm{Time \:} \left(\mathrm{s} \right)$', fontsize = fontsize)
    plt.xscale('log')
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
    transient_plot('fluxcont_WCu_transient_out.csv', interface='D', show=False)
    transient_plot('chempot_WCu_transient_out.csv', linespec='--', filename='WCu_transient.png', title= "W Cu")
    transient_plot('fluxcont_CuEurofer_transient_out.csv', interface='D', show=False)
    transient_plot('chempot_CuEurofer_transient_out.csv', linespec='--', filename='CuEurofer_transient.png', title="Cu Eurofer")
