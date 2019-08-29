<CsoundSynthesizer>
<CsOptions>
-odac -+rtaudio=CoreAudio
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

instr 1
    ain vco2 0.1, 440
    ; ain diskin "input.wav"
    aout sofa ain

    outs aout, aout
endin

schedule(1, 0, -1)

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
