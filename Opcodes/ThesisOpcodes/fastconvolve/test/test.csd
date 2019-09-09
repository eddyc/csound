<CsoundSynthesizer>
<CsOptions>
-odac -+rtaudio=CoreAudio
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 256
nchnls = 2
0dbfs = 1

; gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/real-kemar-l.dat"
; gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/real-kemar-r.dat"
; gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/hrtf-44100-left.dat"
; gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/hrtf-44100-right.dat"
gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/kemar-test-r.dat"
gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/kemar-test-l.dat"

instr 1

    kin init 0

    ain = vco2(0.1, 440)
    ain diskin2 "input.wav", 1, 0, 1

    kKey, kKeyDown sensekey

    if kKeyDown != 0 && kKey != -1 then

        kin = kin + 1

    endif
    aoutL fastconvolve ain, gSpathL, kin
    aoutR fastconvolve ain, gSpathR, kin
    outs(aoutL, aoutR);
endin

schedule(1, 0, -1)

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
