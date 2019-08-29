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

instr 1
    ; ain vco2 0.1, 220
    ain diskin "input.wav"
    kphasor phasor 1/50

    ktf = kphasor * 709
    ktf = round(ktf)

    aoutL sofa ain, gSpathL, ktf
    aoutR sofa ain, gSpathR, ktf

    outs aoutL, aoutR
endin

schedule(1, 0, -1)

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
