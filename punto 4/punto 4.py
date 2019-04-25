import numpy as np
import random
import scipy as sp
#import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.pylab as pyl
import scipy.io.wavfile
from scipy import signal

def clarinet_funct(attack,sustain,release,sampling_frecuenciy,A0,I0):
  
   ta = pyl.arange(0,attack-1/sampling_frecuenciy,1/sampling_frecuenciy)
   y1 = pyl.exp(ta/attack*1.5)-1
   y1 = y1/max(y1)
   y1 = np.append(y1 , pyl.ones([1,round(sustain*sampling_frecuenciy)]))
   tr = pyl.arange(0,(release/2-1/sampling_frecuenciy),1/sampling_frecuenciy)
   y3 = pyl.exp((release/2-tr)/release*3)-1
   y4 = pyl.ones(len(y3))- y3[::-1] 
   y3 = y3/max(y3)/2

   y2 = np.append(y1, pyl.ones([1,round(release*sampling_frecuenciy)]))
   y1 = np.append(y1 ,y4)
   y1 =np.append(y1,y3)
   
   lenght = min(len(y1) ,len(y2))
   y1 = y1[:lenght]
   y2 = y2[:lenght]
   y2 = -I0*y2 + 4
   y1 = A0*y1

   return [y1,y2]      #[A(t),I(t)]


def clarinete (f0,A0,I0,duracion,fsampl):
    fc = 3*f0
    fm = 2*f0
    ts = pyl.arange(0,duracion,1/fsampl) #creo el espacio temporal
    

    [A_t,I_t] = clarinet_funct(0.15*duracion,0.75*duracion,0.1*duracion,fsampl,A0,I0)
    ts =ts[:len(A_t)] #pongo todos los tiempos con la misma cant de puntos (Tengo que ver de hacerlo mejor)
    ym= pyl.sin(2*pyl.pi*fm*ts)     #función modulada
    #Señal FM final con sus componentes que varían en el tiempo
    yc= A_t * pyl.sin(2*pyl.pi*fc *ts +(I_t *ym))
    return yc


def campana(f0,A0,I0,duracion,fsampl):
    ts = pyl.arange(0,duracion,1/fsampl) #creo el espacio temporal
    Tao = duracion/3
    fc = f0
    fm = 2*f0

    I_t=I0*pyl.exp(-ts/Tao) #para la campana 
    A_t =A0 * pyl.exp(-ts/Tao)  #para la campana
    ym= pyl.sin(2*pyl.pi*fm*ts)     #función modulada
    #Señal FM final con sus componentes que varían en el tiempo
    yc= A_t * pyl.sin(2*pyl.pi*fc *ts +(I_t *ym))
    return yc



def violin(f0,A0,I0,duracion,fsampl):
    fc = 2*f0
    fm = 3*f0
    ts = pyl.arange(0,duracion,1/fsampl) #creo el espacio temporal
    

    [A_t,I_t] = clarinet_funct(0.2*duracion,0.7*duracion,0.1*duracion,fsampl,A0,I0) #el ataque es igual
    ts =ts[:len(A_t)] #pongo todos los tiempos con la misma cant de puntos (Tengo que ver de hacerlo mejor)
    ym= pyl.sin(2*pyl.pi*fm*ts)     #función modulada
    #Señal FM final con sus componentes que varían en el tiempo
    yc= A_t * pyl.sin(2*pyl.pi*fc *ts +(I_t *ym))
    return yc


#Mi banco de pruebas para escupir wavs
#

#test = np.concatenate((clarinete(261.626,2,2,1,44100),clarinete(293.625,2,2,1,44100),clarinete(329.628,2,2,1,44100),clarinete(349.228,2,2,1,44100),clarinete(391.995,2,2,1,44100),clarinete(440,2,2,1,44100),clarinete(493.883,2,2,1,44100),clarinete(523.251,2,2,1,44100)))
#escupe archivos wav para escuchar sonidos
#scipy.io.wavfile.write('testclarinete.wav',44100,test)
#scipy.io.wavfile.write('testbell.wav',44100,campana(330,1,5,10,44100))
#scipy.io.wavfile.write('testviolin.wav',44100,violin(330,2,2,5,44100))
