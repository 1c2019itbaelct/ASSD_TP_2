import mido as md
import array
import numpy as np
from punto4 import campana
from punto4 import clarinete
from punto4 import violin
import sounddevice as sd
import matplotlib.pyplot as plt
class myNote:
    def __init__(self,note,velocity,ti):
        self.note=note
        self.tInicial=ti
        self.tFinal=0
        self.dt=0
        self.velocity=velocity

    def calcDt(self):
        self.dt=self.tFinal -self.tInicial



#recive un track, y devuelve un vector de notas
def trackParser (track,tickPerBeat,tempo):
    openNotes=[]
    closedNotes=[]
    currentTime=0
    for x in track:

        currentTime=currentTime+md.tick2second(x.time,tickPerBeat,tempo)
        if(x.type=='note_on'):
            if (x.velocity == 0):#si velocity es cero es igual a un note off

                for y in openNotes:
                    if(y.note==x.note):
                        temp=myNote(y.note,y.velocity,y.tInicial)
                        temp.tFinal=currentTime
                        temp.calcDt()
                        closedNotes.append(temp)
                        openNotes.remove(y)
            else:

                openNotes.append(myNote(x.note,x.velocity,currentTime))
        elif(x.type=='note_off'):
            for y in openNotes:
                if (y.note == x.note):
                    temp = myNote(y.note, y.velocity, y.tInicial)
                    temp.tFinal = currentTime
                    temp.calcDt()
                    closedNotes.append(temp)
                    openNotes.remove(y)


    return closedNotes

def noteToFrec(note):
    return ((2**((note-69)/12)))*440




def trackGenerator(parsedTrack,instrumento,Fs):
    songSize=parsedTrack[len(parsedTrack)-1].tFinal*Fs
    song=np.zeros(int(songSize*1.5))
    length=0
    for x in parsedTrack:
        ti = x.tInicial
        tf = x.tFinal
        dt = tf - ti
        fragmento =sintetizador(noteToFrec(x.note), dt, Fs, instrumento)
        length=int(ti * Fs) + len(fragmento)
        song[int(ti * Fs):length] = song[int(ti * Fs):length] + fragmento
    return song[0:length-1]

def sintetizador(frec,dt,Fs,instrumento):
    if(instrumento=='campana'):
       data=campana(frec, 1, 1, dt, Fs)
    elif(instrumento=='clarinete'):
        data = clarinete(frec, 2, 4, dt, Fs)
    else:
        data=violin(frec, 1, 1, dt, Fs)
    return data







Fs=41100
file = md.MidiFile('concierto.mid')
trackVector = file.tracks
track = trackVector[3]
dataTrack=file.tracks[0]
parsedTrack = trackParser(track,file.ticks_per_beat,(dataTrack[2]).tempo)
parsedTrack2=trackParser(trackVector[6],file.ticks_per_beat,(dataTrack[2]).tempo)


sound1=trackGenerator(parsedTrack,'campana',Fs)
sound2=trackGenerator(parsedTrack2,'violin',Fs)
sund3=np.zeros(min(len(sound1),len(sound2)))
m2=max(abs(sound2.max()),abs(sound2.min()))
m1=max(abs(sound1.max()),abs(sound1.min()))
for i in range(0,len(sund3)-1):
    sund3[i]=sound1[i]/m1+sound2[i]/m2

sund3=sund3/max(abs(sund3.max()),abs(sund3.min()))
print(sund3.max())
print(sund3.min())

#plt.plot(np.arange(0,len(sund3)*(1/Fs),1/Fs),sund3)
#plt.show()
sd.play(sound1,Fs)
sd.wait()
print('hola')