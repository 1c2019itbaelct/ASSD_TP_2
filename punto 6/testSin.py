import sounddevice as sd
import numpy as np
from math import pi
import soundfile as sf
from scipy import signal
import matplotlib.pyplot as plt

Fs =41000 #frecuencia de sampleo
f=200
timeVector= np.arange(0,1,1/Fs)
soundVector= (np.sin(2*pi*f*timeVector) + np.sin(2*50*pi*f*timeVector) + np.sin(2*100*pi*f*timeVector))/3

sd.play(soundVector,Fs)
sd.wait()
sf.write('hola.wav',soundVector,Fs)

f, t, Sxx = signal.spectrogram(soundVector, Fs)
plt.pcolormesh(t, f, Sxx)
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()