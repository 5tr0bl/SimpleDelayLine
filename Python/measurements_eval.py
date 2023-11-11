#import numpy as np
import numpy as np
import os
import plotly.graph_objects as go
import pyfar as pf

def process_and_plot_wav(wav_file_path, plot_path):
    # Load the audio file using pyfar
    signal = pf.io.read_audio(wav_file_path)
    file_path = os.path.splitext(os.path.basename(wav_file_path))[0]

    frequencies = signal.frequencies
    fft = np.fft.fft(signal.time)
    magnitude_spectrum = np.abs(fft)#signal.freq_raw.T 
    phase = np.angle(fft)

    # Create a Plotly figure for magnitude spectrum
    fig_magnitude = go.Figure()
    fig_magnitude.add_trace(go.Scatter(x=frequencies, y=magnitude_spectrum, mode='lines'))
    fig_magnitude.update_layout(title=f'Magnitude Spectrum - {os.path.basename(wav_file_path)}')
    fig_magnitude.write_html(os.path.join(plot_path, f'magnitude_spectrum_{file_path}.html'))

    # Create a Plotly figure for phase
    fig_phase = go.Figure()
    fig_phase.add_trace(go.Scatter(x=frequencies, y=phase.real, mode='lines'))
    fig_phase.update_layout(title=f'Phase Information - {os.path.basename(wav_file_path)}')
    fig_phase.write_html(os.path.join(plot_path, f'phase_information_{file_path}.html'))

path_linear_measurements = "Python\Measurements\Linear"
path_thiran_measurements = "Python\Measurements\Thiran"
path_lagrange_measurements = "Python\Measurements\Lagrange3rd"
measurement_paths = [path_linear_measurements, path_thiran_measurements, path_lagrange_measurements]
path_linear_plots = "Python\Plots\Linear"
path_thiran_plots = "Python\Plots\Thiran"
path_lagrange_plots = "Python\Plots\Lagrange3rd"
plots_paths = [path_linear_plots, path_thiran_plots, path_lagrange_plots]

for measurement_path, plot_path in zip(measurement_paths, plots_paths):
    print(f"Processing files in {measurement_path}:")
    
    # Create the plots folder if it doesn't exist
    os.makedirs(plot_path, exist_ok=True)

    # Iterate over the .wav files in the directory
    for filename in os.listdir(measurement_path):
        if filename.endswith(".wav"):
            wav_file_path = os.path.join(measurement_path, filename)
            print(f"Processing file: {wav_file_path}")

            # Process and plot the wav file
            process_and_plot_wav(wav_file_path, plot_path)