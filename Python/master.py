import numpy as np
import pyfar as pf

resources = "C:\Studium\TU Berlin\Virtuelle Akustik\Resources"
sample_rate = 48000
dirac = pf.signals.deterministic.impulse(sample_rate, sampling_rate=sample_rate)
pf.io.write_audio(dirac, resources + "\dirac_1s_48k.wav")
#exp_sweep = pf.signals.exponential_sweep_time(48000, [20, 20000])
#pf.io.write_audio(exp_sweep, resources + "\sweep_exp_48k.wav")
#pulsed_noise_white = pf.signals.pulsed_noise(48000, 24000, repetitions=3, rms=0.7, spectrum='white', sampling_rate=48000)
#pf.io.write_audio(pulsed_noise_white, resources + "\pulsed_noise_white_48k.wav")
drum_signal = pf.signals.files.drums(48000)
#pf.io.write_audio(drum_signal, resources + "\drums_48k.wav")
#guitar_signal = pf.signals.files.guitar(48000)
#pf.io.write_audio(guitar_signal, resources + "\guitar_48k.wav")

ir_path = resources + "\FABIAN_HRIR_measured_HATO_0.sofa"
hrirs, sample_grid, _ = pf.io.read_sofa(ir_path)

output_path_lateral = resources + "\hrir_lateral.wav"
idx_lateral, _ = sample_grid.find_nearest_k(30, 30, 1, 1, 'sph', 'top_elev', unit='deg', show=True)
hrir_lateral = hrirs[idx_lateral]
hrir_lateral_left = hrirs[idx_lateral].time[0]
hrir_lateral_right = hrirs[idx_lateral].time[1]
hrir_lateral_stereo = np.vstack((hrir_lateral_left, hrir_lateral_right)).T # ???
hrir_lateral_signal = pf.Signal([hrir_lateral_left, hrir_lateral_right], int(hrirs.sampling_rate))
pf.io.write_audio(hrir_lateral_signal, output_path_lateral)

output_path_frontal = resources + "\hrir_frontal.wav"
idx_front, _ = sample_grid.find_nearest_k(0, 0, 1, 1, 'sph', 'top_elev', unit='deg', show=True)
hrir_front = hrirs[idx_front]
hrir_front_left = hrirs[idx_front].time[0]
hrir_front_right = hrirs[idx_front].time[1]
hrir_front_stereo = np.vstack((hrir_front_left, hrir_front_right)).T # ???
hrir_front_signal = pf.Signal([hrir_front_left, hrir_front_right], int(hrirs.sampling_rate))
pf.io.write_audio(hrir_front_signal, output_path_frontal)

drums_convolved_lateral = pf.dsp.convolve(drum_signal, hrirs[idx_lateral])
drums_convolved_lateral_path = resources + "\drums_convolved_lateral.wav"
pf.io.write_audio(drums_convolved_lateral, drums_convolved_lateral_path)
'''
drums_con_l = pf.dsp.convolve(drum_signal, hrir_lateral_left)
drums_con_r = pf.dsp.convolve(drum_signal, hrir_lateral_right)
drums_stereo_signal = pf.Signal([drums_con_l, drums_con_r], int(hrirs.sampling_rate))
drums_convolved_sep_path = resources + "\drums_convolved_sep.wav"
pf.io.write_audio(drums_stereo_signal, drums_convolved_sep_path)

stereo_audio = np.vstack((hrir_lateral_left, hrir_lateral_right)).T
output_path_pf = resources + "\hrir_test_pf.wav"

stereo_signal = pf.Signal([hrir_lateral_left, hrir_lateral_right], int(hrirs.sampling_rate))
pf.io.write_audio(stereo_signal, output_path_pf)
'''