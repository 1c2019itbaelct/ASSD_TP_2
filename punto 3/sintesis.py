from scipy.io import wavfile
from matplotlib import pyplot as plt
import numpy as np
from scipy.fftpack import fft
from scipy import signal
from numpy import arange
import numpy.fft
import sys
from scipy import interpolate
import array as arr
import wave
import operator


def get_file(file):  # devuelve la señal normalizada, la frecuencia de sampleo y el tiempo
    fs, data = wavfile.read(file)
    time = np.arange(len(data)) / float(fs)
    sign = data.T[0]
    sign = sign / max([max(sign), abs(min(sign))])
    return fs, sign, time


def make_fft(sign):  # makes fft of the signal and returns the module of the fft and frecuency
    fft_signal = np.fft.rfft(sign)  # calculate fourier transform (complex numbers list)
    freq = np.fft.rfftfreq(len(sign))
    maximum = max(abs(fft_signal))
    fft_signal_module = abs(fft_signal) / maximum
    return fft_signal_module, freq


def synthesizer(file, frecuency, duration):
    fs, sign, time = get_file(file)

    signal_peak = signal.find_peaks(sign, 0, None, len(sign)/50)  # finds signal peaks
    x_new = np.arange(0, duration, 1 / fs)  # and interpolates them
    interpolated = interpolate.spline(time[signal_peak[0]] * duration/time[len(time)-1], signal_peak[1]['peak_heights'], x_new)  #

    fft_signal_module, freq = make_fft(sign)  # makes fft
    fft_peaks = signal.find_peaks(fft_signal_module[:int((len(fft_signal_module) - 1))], 1)  # and finds
    freq_sumator = frecuency - freq[fft_peaks[0][0]] * fs
    fft_peaks = signal.find_peaks(fft_signal_module[:int((len(fft_signal_module) - 1))], 0.01, None,
                                  freq[fft_peaks[0][0]] * fs)  # its peaks

    i=0
    freq_peaks = fft_peaks[0]
    fft_peaks_new = fft_peaks[1]['peak_heights']
    while fft_peaks[1]['peak_heights'][i] < 1:
        freq_peaks = np.delete(freq_peaks, 0)
        fft_peaks_new = np.delete(fft_peaks_new, 0)
        i = i + 1

    length = 0
    num = np.arange(0, duration, 1 / fs)
    final_signal = 0

    while length <= len(freq_peaks) - 1:  # hace la suma de las senoidales en cada frecuencia con su respectivo modulo, ademas a cada senoidal se la multiplica por la envolvente
        seno = fft_peaks_new[length] * np.sin(2 * np.pi * num * ((length+1) * freq_sumator + freq[freq_peaks[length]] * fs))
        final_signal = final_signal + interpolated * seno
        length = length + 1

    final_signal = final_signal / max([max(final_signal), abs(min(final_signal))])  # normalizo la salida
    wavfile.write("sample.wav", fs, final_signal)

    fft_final, freq_fft_final = make_fft(final_signal)

    plt.xlim(0, 2000)
    plt.plot(freq * fs, fft_signal_module, 'k')
    plt.scatter(freq[freq_peaks] * fs, fft_peaks_new)
    plt.show()

    plt.xlim(0, 2000)
    plt.plot(freq_fft_final * fs, fft_final, 'k')
    plt.show()

    plt.plot(time, sign, 'r')
    # plt.show()

    plt.plot(x_new, interpolated)
    plt.show()

    plt.plot(num, final_signal, 'k')
    plt.show()


if __name__ == "__main__":
    synthesizer("saxophone_Cs4_15_forte_normal.wav", 250, 1.5)
