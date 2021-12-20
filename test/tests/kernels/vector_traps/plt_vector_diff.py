import pandas as pd
import matplotlib.pyplot as plt
data1 = pd.read_csv('simple_out.csv')
data2 = pd.read_csv('simple_vector_out.csv')

plt.plot(data1.time, data1.back_flux, label = 'original')
plt.plot(data2.time, data2.back_flux, label = 'vector implementation')
plt.xlabel('time [s]')
plt.ylabel('flux')
plt.legend()
plt.savefig('vector_regression.png', dpi = 300)
plt.close()
