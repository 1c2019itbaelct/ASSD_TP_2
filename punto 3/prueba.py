# from scipy.io import wavfile
# from matplotlib import pyplot as plt
# import numpy as np
# from scipy.fftpack import fft
# from scipy import signal
# from numpy import arange
# import numpy.fft
# import sys
# from scipy import interpolate
# import array as arr
# import wave
# import operator

# numpy.set_printoptions(threshold=sys.maxsize)
# recreated_signal = 0
#
# violin = "cello_Cs4_05_forte_arco-normal.wav"
# fs, data = wavfile.read(violin)
# time = np.arange(len(data)) / float(fs)
# sign = data.T[0]
# sign = sign / max([max(sign), abs(min(sign))])
# fft_signal = np.fft.rfft(sign)  # calculate fourier transform (complex numbers list)
# freq = np.fft.rfftfreq(len(sign))
# maximum = max(abs(fft_signal))
# fft_signal_module = abs(fft_signal) / maximum
#
# size = len(fft_signal_module)
# peaks = signal.find_peaks(fft_signal_module[:int((size - 1))], 1)
# peaks = signal.find_peaks(fft_signal_module[:int((size - 1))], 0.01, None, freq[peaks[0]] * fs)
#
# signal_peak = signal.find_peaks(sign, 0, None, 1500)
# print(len(signal_peak[0]))
#
# # x_new = np.linspace(time[signal_peak[0]].min(), time[signal_peak[0]].max(),1/fs)
# x_new = np.arange(0, time[len(time) - 1], 1 / fs)
# interpolated = interpolate.spline(time[signal_peak[0]], signal_peak[1]['peak_heights'], x_new)
#
# # num = np.arange(0, time[len(time) - 1], 1 / fs)
# # interpolated = interpolate.pchip(signal_peak[0], signal_peak[1]['peak_heights'])
# # interpolated_new = interpolated(time[signal_peak[0]])
#
# # # print(interpolated.x)
# # frecuency, times, x_signal = signal.spectrogram(sign, fs, 'hanning')
# # n = len(x_signal)-1
# # signals = x_signal[int(n), :]
# # index, value = max(enumerate(signals), key=operator.itemgetter(1))
# # signals = signals/signals[index]
# # signal_peaks, dk = signal.find_peaks(signals, distance = int(np.round(len(sign)/3000)))
# #
# # pk = interpolate.pchip(times[signal_peaks], signals[signal_peaks])
# # interpolated = np.arange(0, time[len(time) - 1], 1/fs)
# # interpolated_new = pk(interpolated)
#
# length = 0
# recreated_signal = []
# num = np.arange(0, time[len(time) - 1], 1 / fs)
# final_signal = 0
# print(len(peaks[0]))
# while length <= len(peaks[0]) - 1:
#     z = 0
#     seno = peaks[1]['peak_heights'][length] * np.sin(2 * np.pi * num * freq[peaks[0][length]] * fs)
#     recreated_signal = []
#     # for x in range (0, len(interpolated_new)-1):
#     #     y = 0
#     #     while y < (len(seno)/len(interpolated_new)) - 1:
#     #         seno[z] = seno[z] * interpolated_new[x]
#     #         z = z + 1
#     #         y = y + 1
#     final_signal = final_signal + interpolated * seno
#     length = length + 1
#
# final_signal = final_signal / max([max(final_signal), abs(min(final_signal))])
# wavfile.write("sample.wav", fs, final_signal)
# ###############plots#########################
#
# plt.xlim(0, 20000)
# plt.plot(freq * fs, fft_signal_module, 'k')
# # plt.show()
# plt.scatter(freq[peaks[0]] * fs, peaks[1]['peak_heights'])
# # plt.scatter(freq[harmonics] * fs, fft_signal_module[harmonics])
# plt.show()
#
# # plt.xlim(0.2, 0.3)
# # print(signal_interpolated.)
#
# plt.plot(time, sign, 'r')
# plt.plot(num, final_signal, 'k')
# # plt.plot(time[signal_peak[0]], interpolated_new)
# plt.show()
#
# # plt.plot(time, data[:, 0], 'r')
# # plt.plot(num, final_signal, 'k')
# plt.plot(x_new, interpolated)
# plt.show()

############################################

# for x in range(0, len(fft_signal_module)):  #en el rango audible
#     if freq[x] <= freq[peaks[0]] * n + 0.00000001 and freq[x] >= freq[peaks[0]] * n - 0.00000001:
#         harmonics = harmonics[:n-1] + [x]
#         n = n +1
#     if freq[x] * fs > 20000:
#         break

# #sudo apt-get install python-pyaudio
#
# PyAudio = pyaudio.PyAudio
#
# #See http://en.wikipedia.org/wiki/Bit_rate#Audio
# BITRATE = 16000 #number of frames per second/frameset.
#
# FREQUENCY = 500 #Hz, waves per second, 261.63=C4-note.
# LENGTH = 1 #seconds to play sound
#
# if FREQUENCY > BITRATE:
#     BITRATE = FREQUENCY+100
# NUMBEROFFRAMES = int(BITRATE * LENGTH)
# RESTFRAMES = NUMBEROFFRAMES % BITRATE
# WAVEDATA = ''
#
# for x in range(NUMBEROFFRAMES):
#  WAVEDATA = WAVEDATA+chr(int(math.sin(x/((BITRATE/FREQUENCY)/math.pi))*127+128))
#
# for x in range(RESTFRAMES):
#  WAVEDATA = WAVEDATA+chr(128)
#
# p = PyAudio()
# stream = p.open(format = p.get_format_from_width(1),
#                 channels = 1,
#                 rate = BITATE,
#                 output = True)
#
# stream.write(WAVEDATA)
# stream.stop_stream()
# stream.close()
# p.terminate()

# violin = "cello_Cs4_15_mezzo-piano_arco-normal.wav"
# fs, data = wavfile.read(violin)
# time = np.arange(len(data)) / float(fs)
# sign = data.T[0]  # this is a two channel soundtrack, I get the first track
# signal_8 = [(ele / 2 ** 8.) * 2 - 1 for ele in sign]  # this is 8-bit track, b is now normalized on [-1,1)
# fft_signal = fft(signal_8)  # calculate fourier transform (complex numbers list)
# size = len(fft_signal) / 2  # you only need half of the fft list (real signal symmetry)
#
# peaks = signal.find_peaks(sign, 100, None, 100)
# print(peaks[0][len(peaks[0]) - 1])
# last = peaks[0][len(peaks[0]) - 1]
# # plt.xlim(0.2,0.3)
# interpolated = interpolate.interp1d(peaks[0], peaks[1]['peak_heights'])
# interpolated_new = interpolated(peaks[0])
# # print (time)
#
# num = np.arange(0, time[len(time) - 1], 1 / fs)
# hola = np.sin(2 * np.pi * num * 1000)
#
# # plt.plot(time, sign, 'r')
# # plt.plot(time[peaks[0]], interpolated_new ,'r')
# # plt.show()
# chau = hola
# z=0
# for x in range (0, len(interpolated_new)-1):
#     y = 0
#     while y < (len(hola)/len(interpolated_new)) - 1:
#         chau[z] = hola[z] * interpolated_new[x]
#         z = z + 1
#         y = y + 1
# wavfile.write("sample.wav", fs, chau)
# plt.xlim(0, time[len(time) - 1], 1/fs)
# plt.plot(num, hola)
# plt.plot(time[peaks[0]], interpolated_new ,'r')
# plt.show()