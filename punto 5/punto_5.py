import numpy as np
import scipy as sp
#import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.pylab as pyl
import scipy.io.wavfile
from scipy import signal


def plot_spectrogram (funcion,fs):
    f, t, Sxx = signal.spectrogram(funcion, fs)
    plt.pcolormesh(t, f, Sxx)
    plt.ylabel('Frequency [Hz]')
    plt.xlabel('Time [sec]')
    plt.show()

def make_sine_wavetable(n_samples, amps, phases, freqs):
    """Makes a wavetable from a sum of sines."""
    t = np.linspace(0, 1, num=n_samples)
    wavetable = np.zeros_like(t)
    for amp, phase, freq in zip(amps, 
                                phases,
                                freqs):
        wavetable += amp * np.sin(np.sin(2 * np.pi * freq * t + phase)) + \
                         amp / 2 * np.sin(np.sin(2 * np.pi * 2 * freq * t + phase))
    return wavetable

def karplus_strong(waveshape, n_samples,stretch_factor):

    samples = []
    current_sample = 0
    previous_value = 0
    while len(samples) < n_samples:
        r = np.random.binomial(1, 1 - 1/stretch_factor)
        if r == 0:
            waveshape[current_sample] =  0.5* (waveshape[current_sample] + previous_value)
        samples.append(waveshape[current_sample])
        previous_value = samples[-1]
        current_sample += 1
        current_sample = current_sample % waveshape.size
    return np.array(samples)



def karplus_strong_drum(waveshape, n_samples,prob):
    samples = []
    current_sample = 0
    previous_value = 0
    while len(samples) < n_samples:
        r = np.random.binomial(1, prob*(1-1/stretch_factor))
        sign = float(r == 1) * 2 - 1
        waveshape[current_sample] = sign * 0.5 * (waveshape[current_sample] + previous_value)
        samples.append(waveshape[current_sample])
        previous_value = samples[-1]
        current_sample += 1
        current_sample = current_sample % waveshape.size
    return np.array(samples)


