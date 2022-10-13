#!/usr/bin/env python3

import mms
df1 = mms.run_spatial('mms_spatial.i', 4, console=False, executable='../../../../achlys-opt')
df2 = mms.run_spatial('mms_spatial.i', 4, 'Mesh/second_order=false','Variables/Mobile/order=FIRST', 'Variables/m2/order=FIRST', 'AuxVariables/Solute/order=FIRST', console=False, executable='../../../../achlys-opt')

fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
fig.plot(df2, label='1st Order', marker='o', markersize=8)
fig.plot(df1, label='2nd Order', marker='o', markersize=8)
fig.save('mms_spatial_orders.png')
