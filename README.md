# mruby-cpuusage

Cross-platform CPU usage statistics for mruby - a lightweight mgem for tracking process CPU time snapshots.

## Description

`mruby-cpuusage` provides a simple, dependency-free way to capture CPU usage snapshots of your mruby process. This is particularly useful for benchmarking and performance monitoring.

## Features

- **Cross-platform**: Works on Linux, macOS, Windows, and other Unix-like systems
- **Zero dependencies**: Uses only standard system APIs
- **Snapshot-based**: Capture CPU time at different points for comparison
- **Accurate**: Provides separate user and system CPU time measurements, also child time where available.

## Supported Platforms

- **Linux**:
- **macOS**:
- **Windows**: 
- **Other Unix**:

## Installation

Add the following line to your `build_config.rb`:

```ruby
conf.gem github: 'Asmod4n/mruby-cpuusage'
```

## Usage

### Basic Usage

```ruby
# Take a snapshot of current CPU usage
snapshot = CPUUsage.snapshot

puts "User time: #{snapshot[:user_time]} seconds"
puts "System time: #{snapshot[:system_time]} seconds"
puts "Total time: #{snapshot[:total_time]} seconds"
```

### Benchmarking Example

```ruby
# Capture baseline
start_snapshot = CPUUsage.snapshot

# Do some CPU-intensive work
result = perform_heavy_computation()

# Capture after work
end_snapshot = CPUUsage.snapshot

# Calculate CPU time used
user_time_used = end_snapshot[:user_time] - start_snapshot[:user_time]
system_time_used = end_snapshot[:system_time] - start_snapshot[:system_time]
total_time_used = end_snapshot[:total_time] - start_snapshot[:total_time]

puts "CPU time consumed:"
puts "  User: #{user_time_used}s"
puts "  System: #{system_time_used}s"
puts "  Total: #{total_time_used}s"
```

### Comparing Different Implementations

```ruby
def benchmark_with_cpu(name)
  start = CPUUsage.snapshot
  yield
  finish = CPUUsage.snapshot
  
  cpu_time = finish[:total_time] - start[:total_time]
  puts "#{name}: #{cpu_time} seconds CPU time"
end

benchmark_with_cpu("Algorithm A") do
  # Implementation A
end

benchmark_with_cpu("Algorithm B") do
  # Implementation B
end
```

## API

### `CPUUsage.snapshot`

Returns a hash containing CPU usage information for the current process:

- `:user_time` (Float) - CPU time spent in user mode, in seconds
- `:system_time` (Float) - CPU time spent in kernel mode, in seconds
- `:total_time` (Float) - Total CPU time (user + system), in seconds

All values are cumulative from process start.

## Technical Details

### Time Measurement

- **User time**: Time spent executing user-space code
- **System time**: Time spent in kernel/system calls on behalf of the process
- **Total time**: Sum of user and system time

### Precision

- Unix systems: Microsecond precision (via `struct rusage`)
- Windows: 100-nanosecond precision (via `FILETIME`)

### Thread Safety

The underlying system calls (`getrusage()` and `GetProcessTimes()`) are thread-safe and report cumulative CPU time across all threads in the process.

## License

Apache-2.0 License - see LICENSE file for details

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
