from conans import ConanFile, CMake

import os

class Standardese(ConanFile):
    name = 'tiny-process-library'
    url = 'https://github.com/sztomi/standardese'
    settings = 'os', 'compiler', 'build_type', 'arch'
    license = 'MIT'
    version = 'v0.3-1'
    exports = '*'
    requires = (
        ('spdlog/0.9.0@memsharded/stable'),
        ('ThreadPool/1.0.0@sztomi/ThreadPool'),
        ('cmark/0.27.1@sztomi/cmark'),
        ('tiny-process-library/1.0.5@sztomi/tiny-process-library'),
        ('Boost/1.60.0@lasote/stable')
    )
    generators = 'cmake'

    def build(self):
        cmake = CMake(self.settings)
        os.mkdir('build')
        os.chdir('build')
        self.run('cmake {} {} -DCMAKE_INSTALL_PREFIX={}'
                    .format(self.conanfile_directory,
                            cmake.command_line,
                            self.package_folder))
        self.run('cmake --build .')

    def package(self):
        self.copy('*.hpp', dst='include')
        self.copy('*.a', dst='lib', keep_path=False)
        self.copy('*.lib', dst='lib', keep_path=False)

    def package_info(self):
        self.cpp_info.libs = [self.name]
