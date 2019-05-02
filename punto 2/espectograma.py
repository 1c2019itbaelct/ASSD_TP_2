import sounddevice as sd
import numpy as np
from math import pi
import soundfile as sf
from scipy import signal
import matplotlib.pyplot as plt

Audio, Fs = sf.read('Youve_Got_A_Friend.wav')
f, t, Sxx = signal.spectrogram(Audio, Fs)
plt.pcolormesh(t, f, Sxx)
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()