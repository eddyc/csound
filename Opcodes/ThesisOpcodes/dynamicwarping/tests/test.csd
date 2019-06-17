<CsoundSynthesizer>
<CsOptions>
-odac -+rtaudio=CoreAudio
</CsOptions>
<CsInstruments>
nchnls = 2
0dbfs = 1
ksmps = 256
sr = 44100

#ifndef MODE
#define MODE #0#
#endif

#ifndef MODEL_PATH
#define MODEL_PATH #"input.pv5"#
#endif

#ifndef PALETTE_PATH
#define PALETTE_PATH #"input.pv5"#
#endif

#ifndef OUTPUT_PATH
#define OUTPUT_PATH #"output.pv5"#
#endif

#ifndef FRAME_COUNT
#define FRAME_COUNT #10#
#endif

#ifndef SEGMENT_COUNT
#define SEGMENT_COUNT #100#
#endif

#ifndef BAND_COUNT
#define BAND_COUNT #1#
#endif

instr 1

    k1 init 0
    imode = $MODE

    kmodelM[][], kmodelF[][] hdf5read $MODEL_PATH, "kmagnitudes*", "kfrequencies*"
    kpaletteM[][], kpaletteF[][] hdf5read $PALETTE_PATH, "kmagnitudes*", "kfrequencies*"
    imodelLength lenarray kmodelM, 1

    if (k1 < imodelLength) then

        kmagsFrame[] getrow kmodelM, k1
    endif

    iframeCount = $FRAME_COUNT
    isegmentCount = $SEGMENT_COUNT
    ibandCount = $BAND_COUNT

    if (iframeCount == -1) then
        iframeCount = int(imodelLength / isegmentCount)
    endif

    schedule(2, iframeCount * (ksmps/sr), -1)


    gkmagnitudes[], gkfrequencies[] frameddw kmagsFrame, iframeCount, isegmentCount, ibandCount, kpaletteM, kpaletteF, imode


    if k1 == imodelLength + iframeCount - 1 then
        event("e", 0, 0)
    endif

    k1 += 1

    fsig tab2pvs gkmagnitudes, gkfrequencies
    aOut pvsynth fsig


    out aOut


endin

instr 2

    kmagnitudes[] = gkmagnitudes
    kfrequencies[] = gkfrequencies
    ihopsize = 256
    ifftsize = 1024
    hdf5write $OUTPUT_PATH, kmagnitudes, kfrequencies, ihopsize, ifftsize

endin

schedule(1, 0 ,-1)

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
