import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab

mu, sigma = 100, 15
x = mu + sigma * np.random.randn(10000)

fig = plt.figure()
#ax = fig.add_subplot(111)
# = fig.add_subplot(111)

# the histogram of the data
#plt.hist([1.0,2.0,3.0,1.0,1.0, 3.0], 6, range=[-1, 5], facecolor='green', alpha=0.75, histtype='stepfilled')
plt.hist([1.0,2.0,3.0,1.0,1.0, 3.0], 6, range=[-1, 5], facecolor='green', alpha=0.75, histtype='stepfilled')
plt.errorbar( [1.5,2.5,3.5],  [3,3,4], [1.2,0.9,1.1], [0,0,0], marker='o', 
              ecolor='black', fmt=None, ms=20, mew=4 )


#    (bins, data) = histOutline(angle, binsIn)
#    pylab.plot(bins, data, 'k-', linewidth=2)

#plt.hist([3.0,1.0,1.0, 3.0], 6, range=[-1, 5], edgecolor='black',  histtype='step', fill=False)
#plt.hist([3,3,1], 5, facecolor='red', alpha=0.2, histtype='stepfilled')

#ax.set_xlabel('Smarts')
#ax.set_ylabel('Probability')
#ax.set_title(r'$\mathrm{Histogram\ of\ IQ:}\ \mu=100,\ \sigma=15$')
#ax.set_xlim(40, 160)

#ax.grid(True)

plt.xlabel(r'$p_T^Z$', ha='right')
plt.ylabel('Event Number')
plt.title('Occurences over time')


plt.savefig("testout.pdf")
plt.savefig("testout.png")
plt.show()
