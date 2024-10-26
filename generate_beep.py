import wave

import numpy as np


def generate_beep(filename, duration=1.0, frequency=440.0, sample_rate=44100, amplitude=0.5):
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    wave_data = amplitude * np.sin(2 * np.pi * frequency * t)

    # Ensure that the waveform is within the 16-bit range
    audio = np.int16(wave_data * 32767)

    # Open a WAV file
    with wave.open(filename, 'w') as wav_file:
        # Set parameters
        nchannels = 1
        sampwidth = 2  # 2 bytes for 16-bit audio
        nframes = len(audio)
        comptype = "NONE"
        compname = "not compressed"
        wav_file.setparams((nchannels, sampwidth, sample_rate, nframes, comptype, compname))

        # Write audio data
        wav_file.writeframes(audio.tobytes())

if __name__ == "__main__":
    generate_beep("beep.wav", duration=1.0, frequency=440.0)
