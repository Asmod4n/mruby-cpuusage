##
## CPUUsage Test
##

assert("CPUUsage module exists") do
  assert_true Object.const_defined?(:CPUUsage)
end

assert("CPUUsage.snapshot returns hash") do
  snapshot = CPUUsage.snapshot
  assert_kind_of Hash, snapshot
end

assert("CPUUsage.snapshot has user_time") do
  snapshot = CPUUsage.snapshot
  assert_true snapshot.has_key?(:user_time)
  assert_kind_of Float, snapshot[:user_time]
  assert_true snapshot[:user_time] >= 0.0
end

assert("CPUUsage.snapshot has system_time") do
  snapshot = CPUUsage.snapshot
  assert_true snapshot.has_key?(:system_time)
  assert_kind_of Float, snapshot[:system_time]
  assert_true snapshot[:system_time] >= 0.0
end

assert("CPUUsage.snapshot has total_time") do
  snapshot = CPUUsage.snapshot
  assert_true snapshot.has_key?(:total_time)
  assert_kind_of Float, snapshot[:total_time]
  assert_true snapshot[:total_time] >= 0.0
end

assert("CPUUsage.snapshot total_time equals user + system") do
  snapshot = CPUUsage.snapshot
  expected_total = snapshot[:user_time] + snapshot[:system_time]
  # Allow for small floating-point differences
  assert_true (snapshot[:total_time] - expected_total).abs < 0.0001
end

assert("CPUUsage.snapshot increases with CPU work") do
  # Take first snapshot
  snapshot1 = CPUUsage.snapshot
  
  # Do some CPU work
  sum = 0
  10000.times { |i| sum += i }
  
  # Take second snapshot
  snapshot2 = CPUUsage.snapshot
  
  # CPU time should increase (or at least not decrease)
  assert_true snapshot2[:total_time] >= snapshot1[:total_time]
end

assert("CPUUsage.snapshot can be called multiple times") do
  s1 = CPUUsage.snapshot
  s2 = CPUUsage.snapshot
  s3 = CPUUsage.snapshot
  
  assert_kind_of Hash, s1
  assert_kind_of Hash, s2
  assert_kind_of Hash, s3
end
