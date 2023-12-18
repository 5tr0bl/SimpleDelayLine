from matplotlib import pyplot as plt
#import numpy as np
import os
#import plotly.graph_objects as go
import pyfar as pf

sample_rate = 48000

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
        
def process_and_plot_wav(wav_file_path, plot_path):
    path_references = "Python\ReferenceInputs"
    file_path = os.path.splitext(os.path.basename(wav_file_path))[0]
    plot_file_name = os.path.join(plot_path, file_path + '.png')
    print(f"Processing file: {file_path}")

    # Get signals as pyfar Signals
    output_signal = pf.io.read_audio(wav_file_path)
    input_reference_path = os.path.join(path_references, 
                                        find_input_reference(wav_file_path=file_path,
                                                             path_references=path_references))
    input_signal = pf.io.read_audio(input_reference_path)
    
    if input_signal.sampling_rate != sample_rate:
        input_signal = pf.dsp.resample(input_signal, sample_rate)

    # Pad zeros to the shorter signal to make both signals have the same length
    max_length = max(output_signal.n_samples, input_signal.n_samples)
    if output_signal.n_samples < max_length:
        output_signal = pf.dsp.pad_zeros(output_signal, max_length - output_signal.n_samples)
    if input_signal.n_samples < max_length:
        input_signal = pf.dsp.pad_zeros(input_signal, max_length - input_signal.n_samples)
    print(f"Length Output: {output_signal.n_samples} Samples.")
    print(f"Length Input: {input_signal.n_samples} Samples.")

    # get regularized inverse of input for --- H = (1/X) * Y
    input_signal_reg_inv = pf.dsp.regularized_spectrum_inversion(input_signal,
                                                                 freq_range=(0, input_signal.sampling_rate/2))

    # divide output/input to obtain the transfer function H
    #transfer_function = output_signal / input_signal

    # H = (1/X) * Y
    transfer_function = input_signal_reg_inv * output_signal

    # plotting
    ax = pf.plot.freq(transfer_function) # returns --> ax : matplotlib.pyplot.axes
    #ax = pf.plot.group_delay(transfer_function)

    plt.close() # close down to avoid plotting in the same fig over and over again
    fig = ax.get_figure() # Get the figure from the existing axes
    fig.suptitle(file_path, fontsize=16)
    fig.savefig(plot_file_name, bbox_inches='tight')

measurement_paths = []
#measurement_paths.append("Python\Measurements\Linear")
#measurement_paths.append("Python\Measurements\Thiran")
#measurement_paths.append("Python\Measurements\Lagrange3rd")
measurement_paths.append("Python\Measurements\Dirac_NoConv_Lagrange3rd")
#measurement_paths.append("Python\Measurements\SeparatedSources_Lagrange3rd")

plots_paths = []
#plots_paths.append("Python\Plots\Linear")
#plots_paths.append("Python\Plots\Thiran")
#plots_paths.append("Python\Plots\Lagrange3rd")
plots_paths.append("Python\Plots\Dirac_NoConv_Lagrange3rd")
#plots_paths.append("Python\Plots\SeparatedSources_Lagrange3rd")

# loop over all measurements
for measurement_path, plot_path in zip(measurement_paths, plots_paths):
    print(f"Processing files in {measurement_path}:")
    os.makedirs(plot_path, exist_ok=True)

    for filename in os.listdir(measurement_path):
        if filename.endswith(".wav"):
            wav_file_path = os.path.join(measurement_path, filename)
            print(f"Processing file: {wav_file_path}")
            process_and_plot_wav(wav_file_path, plot_path)