<CsoundSynthesizer>
<CsOptions>
-odac -+rtaudio=CoreAudio
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 1024
nchnls = 2
0dbfs = 1

instr 1

    ktf init 0

    ain = 0
    kin = 0
    ain = vco2(0.1, 440)
    aout fastconvolve ain, "", kin
    outs(aout, aout);
endin

schedule(1, 0, -1)

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
