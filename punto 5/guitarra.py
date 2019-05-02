
import numpy as np
import scipy as sp
#import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.pylab as pyl
import scipy.io.wavfile
from scipy import signal


class GuitarString:
    def __init__(self, pitch, starting_sample, sampling_freq, stretch_factor):
        """Inits the guitar string."""
        self.pitch = pitch
        self.starting_sample = starting_sample
        self.sampling_freq = sampling_freq
        self.stretch_factor = stretch_factor
        self.init_wavetable()
        self.current_sample = 0
        self.previous_value = 0
        
    def init_wavetable(self):
        """Generates a new wavetable for the string."""
        wavetable_size = self.sampling_freq // int(self.pitch)
        self.wavetable = (2 * np.random.randint(0, 2, wavetable_size) - 1).astype(np.float)
        
    def get_sample(self):
        """Returns next sample from string."""
        if self.current_sample >= self.starting_sample:
            current_sample_mod = self.current_sample % self.wavetable.size
            r = np.random.binomial(1, 1 - 1/self.stretch_factor)
            if r == 0:
                self.wavetable[current_sample_mod] =  0.5 * (self.wavetable[current_sample_mod] + self.previous_value)
            sample = self.wavetable[current_sample_mod]
            self.previous_value = sample
            self.current_sample += 1
        else:
            self.current_sample += 1
            sample = 0
        return sample


class DrumString:
    def __init__(self, pitch, starting_sample, sampling_freq, stretch_factor,probability=0.3):
        """Inits the guitar string."""
        self.pitch = pitch
        self.starting_sample = starting_sample
        self.sampling_freq = sampling_freq
        self.stretch_factor = stretch_factor
        self.init_wavetable()
        self.current_sample = 0
        self.previous_value = 0
        self.probability = probability
        
    def init_wavetable(self):
        """Generates a new wavetable for the string."""
        wavetable_size = self.sampling_freq // int(self.pitch)
        self.wavetable = (2 * np.random.randint(0, 2, wavetable_size) - 1).astype(np.float)
        
    def get_sample(self):
        """Returns next sample from string."""
        if self.current_sample >= self.starting_sample:
            current_sample_mod = self.current_sample % self.wavetable.size
            r = np.random.binomial(1, 1 - 1/self.stretch_factor)
            if r == 0:
                self.wavetable[current_sample_mod] =  0.5 * (self.wavetable[current_sample_mod] + self.previous_value)
            sample = self.wavetable[current_sample_mod]
            self.previous_value = sample
            self.current_sample += 1
        else:
            self.current_sample += 1
            sample = 0
        return sample
        


fs=44100
duration=2

freqs = [82, 130, 164,196,261,330]      #frecuencias de las 6 cuerdas de la guitarra base (mi1)
unit_delay = fs//30
delays = [unit_delay * _ for _ in range(len(freqs))]
stretch_factors = [2 * f/freqs[0] for f in freqs]

strings = []
drums=[]
for freq, delay, stretch_factor in zip(freqs, delays, stretch_factors):
    string = GuitarString(freq, delay, fs, stretch_factor)
    string2=DrumString(freq, delay, fs, stretch_factor)
    strings.append(string)
    drums.append(string2)

guitar_sound_1 = [sum(string.get_sample() for string in strings) for _ in range((int) (fs * duration))]
drum_sound =[sum(drum.get_sample() for drum in drums) for _ in range((int) (fs * duration))]

scipy.io.wavfile.write('test2.wav',fs,np.array(drum_sound))