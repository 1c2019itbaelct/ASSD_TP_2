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


def synthesizer(file):
    fs, sign, time = get_file(file)

    signal_peak = signal.find_peaks(sign, 0, None, 1500)  # finds signal peaks
    x_new = np.arange(0, time[len(time) - 1], 1 / fs)  # and interpolates them
    interpolated = interpolate.spline(time[signal_peak[0]], signal_peak[1]['peak_heights'], x_new)  #

    fft_signal_module, freq = make_fft(sign)  # makes fft
    fft_peaks = signal.find_peaks(fft_signal_module[:int((len(fft_signal_module) - 1))], 1)  # and finds
    fft_peaks = signal.find_peaks(fft_signal_module[:int((len(fft_signal_module) - 1))], 0.01, None,
                                  freq[fft_peaks[0]] * fs)  # its peaks

    length = 0
    num = np.arange(0, time[len(time) - 1], 1 / fs)
    final_signal = 0

    while length <= len(fft_peaks[
                            0]) - 1:  # hace la suma de las senoidales en cada frecuencia con su respectivo modulo, ademas a cada senoidal se la multiplica por la envolvente
        seno = fft_peaks[1]['peak_heights'][length] * np.sin(2 * np.pi * num * freq[fft_peaks[0][length]] * fs)
        final_signal = final_signal + interpolated * seno
        length = length + 1

    final_signal = final_signal / max([max(final_signal), abs(min(final_signal))])  # normalizo la salida
    wavfile.write("sample.wav", fs, final_signal)

    # plt.xlim(0, 20000)
    # plt.plot(freq * fs, fft_signal_module, 'k')
    # plt.scatter(freq[fft_peaks[0]] * fs, fft_peaks[1]['peak_heights'])
    # plt.show()
    #
    # plt.plot(time, sign, 'r')
    # plt.show()
    #
    # plt.plot(x_new, interpolated)
    # plt.show()
    #
    # plt.plot(num, final_signal, 'k')
    # plt.show()


if __name__ == "__main__":
    synthesizer("cello_A3_15_mezzo-piano_arco-normal (1).wav")
