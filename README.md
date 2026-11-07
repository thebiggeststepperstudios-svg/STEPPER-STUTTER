# STEPPER STUTTER

A Windows 10/11 x64 VST3 and standalone rhythmic effect built with JUCE. The plugin combines a tempo-guided half-time texture with a beat-grid stutter and a compact pixel-console interface.

## Download a build

Push this repository to GitHub. The **Windows plugin build** workflow runs on every push. Open the repository's **Actions** tab, select the latest successful run, then download **StepperStutter-vst3** and **StepperStutter-standalone** from its Artifacts section. The VST3 artifact is a ZIP containing the plugin bundle; extract it before installing. The standalone artifact is `StepperStutter.exe`.

## Build in GitHub Actions

The workflow uses `windows-latest`, CMake, MSVC and JUCE 8.0.6 fetched by CMake FetchContent. For a manual run, open **Actions → Windows plugin build → Run workflow**. No local compiler or JUCE installation is needed to obtain the artifacts.

To build locally on a Windows x64 development machine with CMake and Visual Studio C++ tools installed:

```powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

## Install the VST3

Extract `StepperStutter-vst3.zip`. Copy `STEPPER STUTTER.vst3` into the per-user VST3 folder:

`C:\Program Files\Common Files\VST3`

Copy the complete `STEPPER STUTTER.vst3` folder into that location, so the result is `C:\Program Files\Common Files\VST3\STEPPER STUTTER.vst3`. Do not copy only the binary from inside the bundle. Restart or rescan plugins in the DAW. Standalone use requires no installation: run `StepperStutter.exe`.

## Parameters

All plugin controls are automatable and saved with the host project/preset through JUCE's AudioProcessorValueTreeState.

| Parameter | Range / choices | Default |
|---|---|---|
| Half-Time On | Off / On | Off |
| Half-Time Speed | 1/2, 1/4 | 1/2 |
| Glide | 2–250 ms | 35 ms |
| Pitch Drop | Off / On | On |
| Half-Time Mix | 0–100% | 100% |
| Half-Time Output | -24 to +12 dB | 0 dB |
| Stutter On | Off / On | Off |
| Stutter Rate | 1/4, 1/8, 1/16, 1/32, 1/4T, 1/8T, 1/4 dotted, 1/8 dotted | 1/16 |
| Gate | 5–100% | 100% |
| Decay | -12 to +6 dB per repeat | 0 dB |
| Pitch | -12 to +12 semitones per repeat | 0 st |
| Reverse | Off / On | Off |
| Probability | 0–100% | 100% |
| Stutter Mix | 0–100% | 100% |
| Stutter Output | -24 to +12 dB | 0 dB |
| Order | Half-Time → Stutter, Stutter → Half-Time | Half-Time → Stutter |
| Global Mix | 0–100% | 100% |
| Master Output | -24 to +12 dB | 0 dB |
| Manual BPM | 30–300 BPM, used when host tempo unavailable | 120 BPM |
| Scanlines | Off / On (display only) | Off |

## Implementation

Audio formats are mono and stereo. Processing supports host sample rates from 44.1 to 192 kHz and uses preallocated ring and capture buffers, parameter smoothing for wet mix and gain, and short repeat fades. Host tempo is used when supplied; the manual BPM parameter is the fallback. The effect reports a four-second tail and has no algorithmic lookahead latency.

The UI is drawn with native vector rectangles and a restricted retro palette; it does not bundle a third-party font or bitmap. The fixed-size editor is 640 × 400 logical pixels.

