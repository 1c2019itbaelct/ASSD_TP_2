import mido as md
import array
import numpy as np
from punto4 import campana
from punto4 import clarinete
from punto4 import violin
import sounddevice as sd
import soundfile as sf


class myNote:
    def __init__(self,note,velocity,ti):
        self.note=note
        self.tInicial=ti
        self.tFinal=0
        self.dt=0
        self.velocity=velocity

    def calcDt(self):
        self.dt=self.tFinal -self.tInicial


class myTempo:
    def __init__(self):
        self.tickI=0
        self.tickF=0
        self.tempo=0







#trackParser
#recive un track(de mido), tick per beat y tempo
#devuelve un arreglo de myNote con los tiempos en segundos
def trackParser (track,tickPerBeat,tempo):
    openNotes=[]
    closedNotes=[]
    currentTime=0
    currentTick=0
    for x in track:
        currentTick=currentTick+x.time
        currentTime=currentTime+md.tick2second(x.time,tickPerBeat,getTempo(currentTick,tempo))
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



#trackGenerator
#recive un vector de myNotes, instrumento (string, ej violin) y la frecuencia de sampleo(Hz)
#devuelve un vector de amplitudes en funcion del tiempo
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

#sintetizador
#utilizando las funciones desarroladas en el resot del tp
#recive frec(frecuencia del instrumento), duracion del sonido, frecuencia de sampleo(Hz), instrumento (string, ej violin)
#devuelve un vector de amplitudes en funcion del tiempo
def sintetizador(frec,dt,Fs,instrumento):
    if(instrumento=='campana'):
       data=campana(frec, 1, 1, dt, Fs)
    elif(instrumento=='clarinete'):
        data = clarinete(frec, 2, 4, dt, Fs)
    else:
        data=violin(frec, 1, 1, dt, Fs)
    return data

#SintetizadorCancion
#recive path (path al archivo midi), tracks(que tracks se quieren sintetizar ej [4,5,6]) , instrumento
#(vector de instrumentos, que instrumento asigno a cada track, ej ['violin','campana','clarinete']) y Fs frecuencia de sampleo (Hz)
#devuelve un vector de amplitudes en el tiempo
def SintetizadorCancion(path,tracks,instrumento,Fs):
    file = md.MidiFile(path)
    trackVector=file.tracks
    dataTrack=trackVector[0]
    tickPerBeat=0
    tempo=750000
    parsedTrackVector=[]
    soundVector=[]
    maxlenghtSound=0
    sound=np.zeros(10) #se almacena la cancion
    try:
        tickPerBeat = file.ticks_per_beat
    except:
        tickPerBeat=140
    #buco el tempo
    # for x in dataTrack:
    #     if(x.type=='set_tempo'):
    #         tempo=x.tempo
    tempo=tempoVectorGenerator(dataTrack)

    #genero las lineas temporales
    for x in tracks:
        parsedTrackVector.append(trackParser(trackVector[x],tickPerBeat,tempo))
    #a cada liena temporar le agrego un instrumento
    for i in range(len(tracks)):
        instrument='default'
        try:
            instrument=instrumento[i]
        except:
            instrument='default'
        soundVector.append(trackGenerator(parsedTrackVector[i],instrument,Fs))

    #busco el track mas largo
    for x in soundVector:
        maxlenghtSound=max(maxlenghtSound,len(x))

    sound=np.zeros(maxlenghtSound)

    #sumo todos los instrumentos
    for x in soundVector:
        for i in range(0,len(x)-1):
            sound[i]=sound[i]+x[i]

    #normalizo el vector
    sound=sound/(max(abs(max(sound)),abs(min(sound))))
    return sound





def tempoVectorGenerator(dataTrack):
    currentTick=0
    tempoVector=[]

    for x in dataTrack:
        if(x.type=='set_tempo'):
            currentTick=currentTick+x.time
            temp=myTempo()
            temp.tickI=currentTick
            temp.tempo=x.tempo
            tempoVector.append(temp)
            if (len(tempoVector)!=0):
                (tempoVector[len(tempoVector)-2]).tickF=currentTick
    return tempoVector


def getTempo(currentTick,tempoVector):
    tempo=0
    if(len(tempoVector)!=0):
        for x in tempoVector:
            if ((currentTick>=x.tickI)&(currentTick<x.tickF)):
                tempo=x.tempo
                break
            elif (x.tickF==0):
                tempo = x.tempo
                break
    else:
        tempo=tempoVector[0].tempo

    return tempo


Fs=41100
sound=SintetizadorCancion('RodrigoAdagio.mid',[1,2,3],['campana','clarinete','violin'],Fs)

sf.write('RodrigoAdagio.wav',sound,Fs)

sd.play(sound,Fs) # descomentar para reproducir
sd.wait() # descomentar para reproducir
