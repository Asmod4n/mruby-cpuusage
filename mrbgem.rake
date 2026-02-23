MRuby::Gem::Specification.new('mruby-cpuusage') do |spec|
  spec.license = 'MIT'
  spec.author = 'mruby developers'
  spec.summary = 'Cross-platform CPU usage tracking for mruby'
  spec.version = '1.0.0'

  spec.add_dependency 'mruby-time', core: 'mruby-time'
end
