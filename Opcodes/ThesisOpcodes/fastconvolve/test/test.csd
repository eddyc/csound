<CsoundSynthesizer>
<CsOptions>
-odac -+rtaudio=CoreAudio
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 512
nchnls = 2
0dbfs = 1

gSpathL = "/Users/eddyc/Documents/Software/HRTF/Mesh2HRTF/Meshes/Victor/test-left/EvaluationGrid_GeneralTF.sofa"
gSpathR = "/Users/eddyc/Documents/Software/HRTF/Mesh2HRTF/Meshes/Victor/test-right/EvaluationGrid_GeneralTF.sofa"
instr 1

    kin init 0

    ; ain = vco2(0.1, 140)
    ; ain = oscil(0.1, 220, 1)

    ain diskin2 "input.wav", 1, 0, 1
    kline line 0, p3, 710

    printk2 kline

    kKey, kKeyDown sensekey

    if kKeyDown != 0 && kKey != -1 then

        kin = kin + 1

    endif
    ; aoutL fastconvolve ain, gSpathL, kline, "IR", 0
    ; aoutR fastconvolve ain, gSpathR, kline, "IR", 1
    aoutL fastconvolve ain, gSpathL, kline, "TF", 0
    aoutR fastconvolve ain, gSpathR, kline, "TF", 0
    outs(aoutL, aoutR);
endin

; schedule(1, 0, 1)

</CsInstruments>
<CsScore>
f1 0 128 10 1                                          ; Sine with a small amount of data
i1 0 120
</CsScore>
</CsoundSynthesizer>
