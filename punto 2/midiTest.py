import mido as md
import array
import numpy as np
from punto4 import clarinete
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
def trackParser (track):
    openNotes=[]
    closedNotes=[]
    currentTime=0
    for x in track:
        currentTime=currentTime+x.time
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
Fs=41100




file = md.MidiFile('test.mid')
trackVector = file.tracks
track = trackVector[1]
parsedTrack = trackParser(track)
songSize=(md.tick2second(parsedTrack[len(parsedTrack)-1].tFinal,file.ticks_per_beat,300000))*Fs
song=np.zeros(int(songSize*1.5))
for x in parsedTrack:
    ti =md.tick2second(x.tInicial,file.ticks_per_beat,300000)
    tf=md.tick2second(x.tInicial,file.ticks_per_beat,300000)
    dt=tf-ti
    #fragmento=clarinete(noteToFrec(x.note),1,4,dt,Fs)
    song[int(ti*Fs):int(tf*Fs)]=song[int(ti*Fs):int(tf*Fs)]#+fragmento


print('hola')