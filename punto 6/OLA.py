import numpy as np
import math as mt
import soundfile as sf
import sounddevice as sd
import matplotlib.pyplot as plt
from math import pi
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
    def tao1(self,i): #funcion de mapeo lienal de M a N
        #return(i*self.k)
        #return(i*0)
        #in_min=self.bw(self.sigma.min())
        in_min=0
        #self.test=self.ew(self.gama[(len(self.gama)-2)])
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
        self.n=len(self.x)
        self.k=k
        self.m=int(self.n * self.k)
        self.y=np.zeros(self.m)
        self.wl=int((1/20)*41000) #genero el tamanio de la vantana para qeu entr un périodo de la senial mas grabe
        self.nu=(1-self.o)*self.wl
        self.gama=np.zeros(mt.ceil(self.m/self.nu))
        self.sigma=np.zeros(len(self.gama))
        self.gama[0] = 1
        self.w=np.hanning(self.wl)
        #compute gamma
        for i in range(1,len(self.gama)-1):
            self.gama[i]=self.gama[i-1] + self.nu
            #print(self.gama[i])

        #compute sigma

        for i in range(0,len(self.sigma)-1):
            self.sigma[i]=self.tao1(self.gama[i])

            #print(self.sigma[i])



        #overlap and add


        for i in range(2,len(self.sigma)-2):
            frame=self.myProduct(self.x[self.bw(self.sigma[i]): self.ew(self.sigma[i])],self.w)
 #           if( (self.ew(self.gama[i])) >= (len(self.x))):
#                print("tu vieja")
            if(len(frame)!=self.wl):
                frame=np.concatenate((frame,np.zeros((self.wl-len(frame)))))
            #print(len(self.y[self.bw(self.gama[i]):(self.ew(self.gama[i]),self.m)]))
 #           print('min '+str(self.bw(self.gama[i]))+' max '+str(self.ew(self.gama[i]))+' sigma ' +'min '+str(self.bw(self.sigma[i]))+' max '+str(self.ew(self.sigma[i])))
            #print(len(frame))

            #print(len(self.y[self.bw(self.gama[i]): (self.ew(self.gama[i]))]))
            self.y[self.bw(self.gama[i]): (self.ew(self.gama[i]))] =  self.y[self.bw(self.gama[i]): (self.ew(self.gama[i]))] + frame

Audio, Fs = sf.read('guitarra.wav')

Fs =41000 #frecuencia de sampleo
f=200
timeVector= np.arange(0,1,1/Fs)
#Audio= (np.sin(2*pi*f*timeVector) + np.sin(2*50*pi*f*timeVector) + np.sin(2*100*pi*f*timeVector))/3
#Audio= np.sin(2*pi*f*timeVector)

abc=ola()
abc.run(Audio,4)
sd.play(abc.y,Fs)
sd.wait()
#sf.write('out.wav',abc.y,Fs)



n = len(Audio)
timeVector = np.arange(0, n*(1/Fs), 1/Fs)

plt.figure(1)
plt.plot(timeVector,Audio)

n2= len(abc.y)
timeVector = np.arange(0, n2*(1/Fs), 1/Fs)
plt.figure(2)
plt.plot(timeVector,abc.y)
plt.show()







