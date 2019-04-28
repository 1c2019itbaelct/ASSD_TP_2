import numpy as np
import math as mt
import soundfile as sf
import sounddevice as sd
import matplotlib.pyplot as plt
from math import pi
from numpy.fft import fft, rfftfreq ,fftfreq, ifft
class ola:
    def _init_(self):

        self.k #factor de estiramiento o contrimiento de la secuencia x ,en veces
        self.n #tamano de la muestra x
        self.m#tamano final de y
        self.x #vecotr de muestras originales
        self.y #vector final de muestras
        self.o  #porcentaje de overlpping
        self.wl #tmano de la ventana
        self.w #vecotr con los valores de la ventana de tamanio wl
        self.gama
        self.sigma
        self.nu
        self.test
    def tao1(self,i):
        in_min=0
        in_max=self.ew(self.gama[(len(self.gama)-2)])
        out_min=0
        return ((i - in_min) * (self.n - out_min) / (in_max - in_min) + out_min)

    def myProduct(self,x,y):
        temp=np.zeros(min(len(x),len(y)))
        for i in range(0,len(temp)-1):
            temp[i] = (x[i])*(y[i])
        return temp

    def bw(self,i):
        return mt.floor(i- ((self.wl -1)/2))

    def ew(self,i):
        return mt.floor(i+ (self.wl/2))

    def run(self,x,k):
        self.o=0.5
        self.x=x
        self.n=len(self.x) #longitud del vector de entrada
        self.k=k #factor de escalamiento
        self.m=int(self.n * self.k) #longitud del vector de slida
        self.y=np.zeros(self.m) #vector de salida
        self.wl=int((1/20)*41000) #genero el tamanio de la vantana para qeu entr un périodo de la senial mas grabe
        self.nu=(1-self.o)*self.wl #overlapping
        self.gama=np.zeros(mt.ceil(self.m/self.nu))
        self.sigma=np.zeros(len(self.gama))
        self.gama[0] = 1
        self.w=np.hanning(self.wl)
        #calculo gamma
        for i in range(1,len(self.gama)-1):
            self.gama[i]=self.gama[i-1] + self.nu

        #calculo sigma

        for i in range(0,len(self.sigma)-1):
            self.sigma[i]=self.tao1(self.gama[i])

        #overlap and add
        try:
            for i in range(2,len(self.sigma)-2):
                frame=self.myProduct(self.x[self.bw(self.sigma[i]): self.ew(self.sigma[i])],self.w)
                if(len(frame)!=self.wl):
                    frame=np.concatenate((frame,np.zeros((self.wl-len(frame)))))
                #print('min '+str(self.bw(self.gama[i]))+' max '+str(self.ew(self.gama[i]))+' sigma ' +'min '+str(self.bw(self.sigma[i]))+' max '+str(self.ew(self.sigma[i])))
                self.y[self.bw(self.gama[i]): (self.ew(self.gama[i]))] =  self.y[self.bw(self.gama[i]): (self.ew(self.gama[i]))] + frame
        except:
            print('El factor de escalamiento no funciona')


Fs=41000 #frecuencia de sampleo
f=20
timeVector= np.arange(0,1,1/Fs)
#Audio, Fs = sf.read('guitarra.wav')
#Audio= (np.sin(2*pi*f*timeVector) + np.sin(2*50*pi*f*timeVector) + np.sin(2*100*pi*f*timeVector))/3
Audio= np.sin(2*pi*f*timeVector)

abc=ola()
abc.run(Audio,2)
#sd.play(abc.y,Fs)
#sd.wait()
#sf.write('out.wav',abc.y,Fs)



n = len(Audio)
timeVector = np.arange(0, n*(1/Fs), 1/Fs)

n = len(Audio)
frecVector = fftfreq(n)
espectroVector = fft(Audio)
mask = frecVector>0
truefft=2.0*np.abs(espectroVector/n)

plt.figure(3)
plt.plot(frecVector[mask],truefft[mask],linewidth=4)
n1=len(abc.y)
frecVector1 = fftfreq(n1)
espectroVector1 = fft(abc.y)
mask1 = frecVector1>0
truefft1=2.0*np.abs(espectroVector1/n)
plt.plot(frecVector1[mask1],truefft1[mask1],linewidth=3)
plt.xlabel('Fs'+'['+str(Fs)+'Hz'+']')
plt.ylabel('Potencia')







plt.figure(1)

plt.plot(timeVector,Audio)

n2= len(abc.y)
timeVector = np.arange(0, n2*(1/Fs), 1/Fs)
#plt.figure(2)

plt.plot(timeVector,abc.y)
plt.xlabel('Tiempo[s]')
plt.ylabel('Amplitud')


plt.show()







