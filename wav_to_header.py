#!/usr/bin/env python3
"""
Convert WAV files to C header files for embedding in firmware
"""

import wave
import struct
import os
import sys

def wav_to_header(wav_path, var_name):
    """
    Convert WAV file to C header with PROGMEM array

    Args:
        wav_path: Path to WAV file
        var_name: C variable name (e.g., "kick_sample")

    Returns:
        Tuple of (header_content, sample_rate, num_samples)
    """
    with wave.open(wav_path, 'rb') as wav:
        sample_rate = wav.getframerate()
        n_frames = wav.getnframes()
        n_channels = wav.getnchannels()
        sampwidth = wav.getsampwidth()

        # Read raw data
        raw_data = wav.readframes(n_frames)

        # Convert to 16-bit samples
        if sampwidth == 2:
            samples = struct.unpack(f'<{n_frames * n_channels}h', raw_data)
        else:
            print(f"ERROR: Expected 16-bit WAV, got {sampwidth*8}-bit")
            return None

        # Convert to mono if stereo
        if n_channels == 2:
            samples = [samples[i] for i in range(0, len(samples), 2)]

        num_samples = len(samples)

        # Generate header content
        header = f"""// Auto-generated from {os.path.basename(wav_path)}
// Sample rate: {sample_rate} Hz, Samples: {num_samples}, Size: {num_samples * 2} bytes

#ifndef {var_name.upper()}_H
#define {var_name.upper()}_H

#include <Arduino.h>

const uint32_t {var_name}_rate = {sample_rate};
const uint32_t {var_name}_length = {num_samples};

const int16_t {var_name}_data[] PROGMEM = {{
"""

        # Write sample data (8 values per line)
        for i in range(0, num_samples, 8):
            line_samples = samples[i:i+8]
            line = "  " + ", ".join(f"{s:6d}" for s in line_samples)
            if i + 8 < num_samples:
                line += ","
            header += line + "\n"

        header += f"""}};

#endif // {var_name.upper()}_H
"""

        return header, sample_rate, num_samples

def process_samples(input_dir, output_dir):
    """
    Process all WAV files in directory and create headers

    Args:
        input_dir: Directory with WAV files
        output_dir: Output directory for headers
    """
    os.makedirs(output_dir, exist_ok=True)

    # Map filenames to variable names
    samples = {
        'kick.wav': 'sample_808_kick',
        'snare.wav': 'sample_808_snare',
        'hat.wav': 'sample_808_hat',
        'tom.wav': 'sample_808_tom'
    }

    print(f"\nConverting WAV files to C headers...")
    print(f"Input:  {input_dir}")
    print(f"Output: {output_dir}\n")

    total_size = 0

    for filename, var_name in samples.items():
        wav_path = os.path.join(input_dir, filename)

        if not os.path.exists(wav_path):
            print(f"⚠ Skipping {filename} (not found)")
            continue

        header_content, sample_rate, num_samples = wav_to_header(wav_path, var_name)

        if header_content:
            # Write header file
            header_filename = f"{var_name}.h"
            header_path = os.path.join(output_dir, header_filename)

            with open(header_path, 'w') as f:
                f.write(header_content)

            size_bytes = num_samples * 2
            total_size += size_bytes

            print(f"✓ {filename:12s} → {header_filename:25s} ({sample_rate:5d} Hz, {num_samples:6d} samples, {size_bytes:7d} bytes)")

    print(f"\nTotal size: {total_size} bytes ({total_size/1024:.1f} KB)")
    print(f"\n✓ Headers saved to: {output_dir}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 wav_to_header.py <input_directory> [output_directory]")
        print("\nExample:")
        print('  python3 wav_to_header.py "/Users/me/Desktop/Downsample This" ./src/samples')
        sys.exit(1)

    input_dir = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else "./src/samples"

    if not os.path.exists(input_dir):
        print(f"ERROR: Input directory not found: {input_dir}")
        sys.exit(1)

    process_samples(input_dir, output_dir)

if __name__ == "__main__":
    main()
