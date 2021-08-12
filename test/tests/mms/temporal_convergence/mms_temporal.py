#!/usr/bin/env python3

import mms
df1 = mms.run_temporal('mms_temporal.i', 6, console=False, executable='../../../../achlys-opt')
fig = mms.ConvergencePlot(xlabel='$\Delta t$', ylabel='$L_2$ Error')
fig.plot(df1, label='1st Order (Implicit Euler)', marker='o', markersize=8)
fig.save('mms_temporal.png')

#TESTING
df1.to_csv('mms_temporal.csv')

