#!/usr/bin/env python3

import mms
df1 = mms.run_spatial('mms_spatial.i', 4, console=False, executable='../../../../achlys-opt')
fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
fig.plot(df1, label='1st Order', marker='o', markersize=8)
fig.save('mms_spatial.png')

#TESTING
df1.to_csv('mms_spatial.csv')

