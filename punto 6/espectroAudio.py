import sounddevice as sd
import numpy as np
from math import pi
import soundfile as sf
from scipy import signal
import matplotlib.pyplot as plt
from numpy.fft import fft, rfftfreq ,fftfreq, ifft
from scipy.signal import argrelextrema

Audio, Fs = sf.read('guitarra.wav')
n = len(Audio)
timeVector = np.arange(0, n*(1/Fs), 1/Fs)
frecVector = fftfreq(n)
espectroVector = fft(Audio)
mask = frecVector>0
truefft=2.0*np.abs(espectroVector/n)
plt.plot(frecVector[mask],truefft[mask])
plt.show()

esta=frecVector[argrelextrema(truefft[mask], np.greater)]*Fs
print(esta[esta>100])
#f, t, Sxx = signal.spectrogram(Audio, Fs)
#plt.pcolormesh(t, f, Sxx)
#plt.ylabel('Frequency [Hz]')
#plt.xlabel('Time [sec]')
#plt.show()
