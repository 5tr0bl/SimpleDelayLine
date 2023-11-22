#import numpy as np
#import numpy as np
import os
#import plotly.graph_objects as go
import pyfar as pf

def find_input_reference(wav_file_path, path_references):
    wav_prefix = os.path.basename(wav_file_path).split('_')[0]
    for ref_filename in os.listdir(path_references):
        ref_prefix = ref_filename.split('_')[0]
        if ref_prefix == wav_prefix:
            print(f"Reference input file: {ref_filename}")
            return ref_filename
    # FileNotFoundError
    print(f"No input reference found for {wav_file_path}")
    return None

## Local scope problems... do not use right now
def pad_length_mismatch(signal_a, signal_b):
    # Pad zeros to the shorter signal to make both signals have the same length
    max_length = max(signal_a.n_samples, signal_b.n_samples)
    if signal_a.n_samples < max_length:
        pf.dsp.pad_zeros(signal_a, max_length - signal_a.n_samples)
    if signal_b.n_samples < max_length:
        pf.dsp.pad_zeros(signal_b, max_length - signal_b.n_samples)
        
def process_and_plot_wav(wav_file_path): #, plot_path):
    path_references = "Python\ReferenceInputs"
    file_path = os.path.splitext(os.path.basename(wav_file_path))[0]
    print(f"Processing file: {file_path}")

    # Get signals as pyfar Signals
    output_signal = pf.io.read_audio(wav_file_path)
    input_reference_path = os.path.join(path_references, 
                                        find_input_reference(wav_file_path=file_path,
                                                             path_references=path_references))
    input_signal = pf.io.read_audio(input_reference_path)
    
    # Pad zeros to the shorter signal to make both signals have the same length
    max_length = max(output_signal.n_samples, input_signal.n_samples)
    if output_signal.n_samples < max_length:
        output_signal = pf.dsp.pad_zeros(output_signal, max_length - output_signal.n_samples)
    if input_signal.n_samples < max_length:
        input_signal = pf.dsp.pad_zeros(input_signal, max_length - input_signal.n_samples)
    print(f"Length Output: {output_signal.n_samples} Samples.")
    print(f"Length Input: {input_signal.n_samples} Samples.")

    # divide output/input to obtain the transfer function
    transfer_function = output_signal / input_signal # regularize/invert H here?

    pf.plot.two_d.freq_group_delay_2d(transfer_function) # how to plot as graph in pyfar?

    '''
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
    '''

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
            process_and_plot_wav(wav_file_path) #, plot_path)