import os
from distutils.core import setup, Extension

curr_dir = os.path.abspath(os.path.dirname(__file__))

basestation = Extension(
    'basestation',
    include_dirs=['/usr/local/include', os.path.join(curr_dir, 'includes')],
    libraries=['libstdc++', 'rf24', 'wiringPi'],
    library_dirs=['/usr/local/lib', '/usr/lib'],
    sources=['src/' + f for f in os.listdir(
        os.path.join(curr_dir, 'src')) if f.endswith('.cpp')])

setup(name='basestation',
      version='1.0',
      author='Benjamin Jacobs',
      author_email='benjammin1100@gmail.com',
      ext_modules=[basestation])
