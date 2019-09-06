<CsoundSynthesizer>
<CsOptions>
-odac -+rtaudio=CoreAudio
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Mesh2HRTF/Meshes/KemarL/EvaluationGrid.sofa"
gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Mesh2HRTF/Meshes/KemarR/EvaluationGrid.sofa"
; gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/kemar-test-l.dat"
; gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/kemar-test-r.dat"
gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/hrtf-44100-left.dat"
gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/hrtf-44100-right.dat"
; gSpathL = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/real-kemar-l.dat"
; gSpathR = "/Users/eddyc/Documents/Software/HRTF Project/Csound/DATFiles/real-kemar-r.dat"
instr 1

    ktf init 0
    ain vco2 0.1, 220
        ain diskin2 "input.wav", 1, 0, 1

    iSlice = 4

    kElevationMeasurementCount[] fillarray 56, 60, 72, 72, 72, 72, 72, 60, 56, 45, 36, 24, 12, 1
    kElevationMeasurementIndices[] fillarray 0, 56, 116, 188, 260, 332, 404, 476, 536, 592, 637, 673, 697, 709

    kStartIndex = kElevationMeasurementIndices[iSlice]
    kCount = kElevationMeasurementCount[iSlice]
    kphasor phasor 2/10

    ; ktf = kphasor * kCount + kStartIndex
    ; ktf = round(ktf)

    kKey, kKeyDown sensekey

    if kKeyDown != 0 && kKey != -1 then

        ktf = ktf + 1

    endif

    printk2 ktf

    aoutL sofa ain, gSpathL, ktf
    aoutR sofa ain, gSpathR, ktf

    ; printk2(k(aoutL))
    ; aoutL = ain
    ; aoutR = ain
    outs aoutL, aoutR
endin

schedule(1, 0, -1)

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
