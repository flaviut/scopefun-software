# Trigger Modes

## Immediate

Cosntantly shows the output of the oscilliscope on the screen. This means that
a reading is taken every clock cycle.

## Auto

Shows the output of the oscliliscope on the screen every time the trigger is
hit.

If the trigger is not hit for a certian amount of time, then an artificial
trigger is inserted to show the current waveform.

This amount of time varies depending on the sampling rate, but ranges from 10s
(at 0.5Hz), to 1ms (at 100MHz).

## Single

Shows the output at the point in time that the trigger is hit. The trigger
doesn't do another attempt, it stops until it is re-armed.

## Normal

Shows the output at the point in time that the trigger is hit. The trigger is
automatially re-armed after each hit.
