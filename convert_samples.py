#!/usr/bin/env python3
"""
WAV Sample Converter for Lab Beat Machine
Converts 24-bit 44100Hz WAV files to 16-bit 22050Hz for optimal M5Cardputer playback
"""

import os
import wave
import struct
import sys

def convert_wav(input_path, output_path, target_rate=22050):
    """
    Convert WAV file to 16-bit mono at target sample rate

    Args:
        input_path: Source WAV file
        output_path: Destination WAV file
        target_rate: Target sample rate (default 22050 Hz)
    """
    print(f"Converting: {input_path}")

    # Read source file
    with wave.open(input_path, 'rb') as wav_in:
        # Get source properties
        channels = wav_in.getnchannels()
        sampwidth = wav_in.getsampwidth()
        framerate = wav_in.getframerate()
        nframes = wav_in.getnframes()

        print(f"  Source: {channels}ch, {sampwidth*8}-bit, {framerate}Hz, {nframes} frames")

        # Read all frames
        raw_data = wav_in.readframes(nframes)

        # Convert based on bit depth
        if sampwidth == 3:  # 24-bit
            # Convert 24-bit to 16-bit samples
            samples = []
            for i in range(0, len(raw_data), 3):
                # Read 3 bytes as little-endian 24-bit signed int
                byte1 = raw_data[i]
                byte2 = raw_data[i+1]
                byte3 = raw_data[i+2]

                # Combine to 24-bit signed value
                sample24 = (byte1) | (byte2 << 8) | (byte3 << 16)
                if sample24 & 0x800000:  # Sign extend
                    sample24 |= 0xFF000000
                sample24 = struct.unpack('i', struct.pack('I', sample24))[0]

                # Convert to 16-bit (divide by 256)
                sample16 = sample24 >> 8
                # Clamp to 16-bit range
                sample16 = max(-32768, min(32767, sample16))
                samples.append(sample16)

        elif sampwidth == 2:  # 16-bit
            # Already 16-bit, just unpack
            samples = list(struct.unpack(f'<{nframes}h', raw_data))
        else:
            print(f"  ERROR: Unsupported bit depth: {sampwidth*8}-bit")
            return False

        # Handle stereo to mono conversion
        if channels == 2:
            mono_samples = []
            for i in range(0, len(samples), 2):
                # Average left and right channels
                mono_samples.append((samples[i] + samples[i+1]) // 2)
            samples = mono_samples

        # Downsample if needed
        if framerate != target_rate:
            ratio = framerate / target_rate
            new_samples = []
            for i in range(int(len(samples) / ratio)):
                # Simple nearest-neighbor downsampling
                src_idx = int(i * ratio)
                new_samples.append(samples[src_idx])
            samples = new_samples
            print(f"  Downsampled from {framerate}Hz to {target_rate}Hz")

        # Normalize to use full dynamic range
        if samples:
            max_val = max(abs(min(samples)), abs(max(samples)))
            if max_val > 0:
                scale = 32767.0 / max_val * 0.95  # Leave 5% headroom
                samples = [int(s * scale) for s in samples]
                print(f"  Normalized (scale: {scale:.2f})")

        # Write output file
        with wave.open(output_path, 'wb') as wav_out:
            wav_out.setnchannels(1)  # Mono
            wav_out.setsampwidth(2)  # 16-bit
            wav_out.setframerate(target_rate)
            wav_out.setnframes(len(samples))
            wav_out.writeframes(struct.pack(f'<{len(samples)}h', *samples))

        out_size = os.path.getsize(output_path)
        in_size = os.path.getsize(input_path)
        print(f"  Output: 1ch, 16-bit, {target_rate}Hz, {len(samples)} frames")
        print(f"  Size: {in_size} -> {out_size} bytes ({100*out_size/in_size:.1f}%)")
        print(f"  ✓ Saved: {output_path}\n")

    return True

def process_directory(directory, target_rate=22050, backup=True):
    """
    Process all WAV files in a directory

    Args:
        directory: Directory containing WAV files
        target_rate: Target sample rate
        backup: Create .bak backups of original files
    """
    if not os.path.exists(directory):
        print(f"Directory not found: {directory}")
        return

    wav_files = [f for f in os.listdir(directory) if f.endswith('.wav')]

    if not wav_files:
        print(f"No WAV files found in: {directory}")
        return

    print(f"\n{'='*60}")
    print(f"Processing {len(wav_files)} files in: {directory}")
    print(f"{'='*60}\n")

    for filename in wav_files:
        input_path = os.path.join(directory, filename)

        # Create backup
        if backup:
            backup_path = input_path + '.bak'
            if not os.path.exists(backup_path):
                import shutil
                shutil.copy2(input_path, backup_path)
                print(f"Backup created: {backup_path}")

        # Convert in place
        temp_path = input_path + '.tmp'
        if convert_wav(input_path, temp_path, target_rate):
            os.replace(temp_path, input_path)
        else:
            if os.path.exists(temp_path):
                os.remove(temp_path)

def main():
    # Target sample rate for embedded playback
    # 22050 Hz is half of 44100 Hz - good for embedded systems
    target_rate = 22050

    # Check for command line arguments
    if len(sys.argv) > 1:
        # User provided path(s)
        for path in sys.argv[1:]:
            if os.path.isdir(path):
                process_directory(path, target_rate)
            elif os.path.isfile(path) and path.endswith('.wav'):
                # Single file conversion
                temp_path = path + '.tmp'
                backup_path = path + '.bak'
                if not os.path.exists(backup_path):
                    import shutil
                    shutil.copy2(path, backup_path)
                    print(f"Backup created: {backup_path}")
                if convert_wav(path, temp_path, target_rate):
                    os.replace(temp_path, path)
            else:
                print(f"Skipping: {path} (not a directory or WAV file)")
    else:
        # Try to find SD card
        sd_root = None

        # Check common macOS mount points
        if os.path.exists("/Volumes/NO NAME"):
            sd_root = "/Volumes/NO NAME"
        elif os.path.exists("/Volumes"):
            volumes = [v for v in os.listdir("/Volumes") if v != "Macintosh HD"]
            if volumes:
                print(f"Found volumes: {volumes}")
                print(f"Using: {volumes[0]}")
                sd_root = os.path.join("/Volumes", volumes[0])

        if sd_root is None:
            print("ERROR: No SD card detected and no path provided")
            print("\nUsage:")
            print("  python3 convert_samples.py <directory>")
            print("  python3 convert_samples.py <file.wav>")
            print("\nExamples:")
            print("  python3 convert_samples.py /Volumes/SDCARD/mp3s/lbm/808")
            print("  python3 convert_samples.py ./samples/*.wav")
            sys.exit(1)

        # Process 808 samples
        dir_808 = os.path.join(sd_root, "mp3s", "lbm", "808")
        process_directory(dir_808, target_rate)

        # Process USER samples
        dir_user = os.path.join(sd_root, "mp3s", "lbm", "user")
        process_directory(dir_user, target_rate)

    print("\n" + "="*60)
    print("Conversion complete!")
    print("="*60)
    print("\nOriginal files backed up with .bak extension")
    print(f"All samples converted to: 16-bit mono @ {target_rate} Hz")
    print("\nTo restore originals: rm *.wav && rename 's/.bak$//' *.bak")

if __name__ == "__main__":
    main()
