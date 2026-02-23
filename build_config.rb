MRuby::Build.new do |conf|
  toolchain :gcc
  enable_debug
  conf.enable_debug
  #conf.enable_sanitizer "address,undefined,leak"
  conf.cc.defines << 'MRB_UTF8_STRING'
  conf.cxx.defines << 'MRB_UTF8_STRING'
  conf.enable_test
  conf.enable_bintest
  conf.cc.flags << '-Os' << '-flto'
  conf.cxx.flags << '-Os' << '-flto'
  conf.linker.flags << "-flto"
  conf.gem core: 'mruby-bin-mirb'
  conf.gem File.expand_path(File.dirname(__FILE__))
end
1
