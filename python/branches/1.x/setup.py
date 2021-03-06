import sys
from deps import *
from distutils.core import setup, Extension

reqglibver = '2.2.0'
reqopenhpiver = '2.10.0'
reqswigver = '1.3.29'
reqpythonver = '2.3.0'

check = check_glibver(reqglibver)
if check:
	print 'GLIB >= %s is required for this extension.' % (reqglibver)
	print 'Found GLIB version %s. Cannot proceed.' % (check)
	sys.exit(1)

check = check_openhpiver(reqopenhpiver)
if check:
	print 'OpenHPI >= %s is required for this extension.' % (reqopenhpiver)
	print 'Found OpenHPI version %s. Cannot proceed.' % (check)
	sys.exit(1)

check = check_swigver(reqswigver)
if check:
	print 'SWIG >= %s is required to build this extension.' % (reqswigver)
	print 'Found SWIG version %s. Cannot proceed.' % (check)
	sys.exit(1)

check = check_pythonver(reqpythonver)
if check:
	print 'Python >= %s is required for this extension.' % (reqpythonver)
	print 'Found Python version %s. Cannot proceed.' % (check)
	sys.exit(2)

includes = ['/usr/include/openhpi', '/usr/local/include/openhpi']
includes += get_glib_cflags()

setup(name='py-openhpi',
      version='1.2',
      description='Python extension for OpenHPI',
      author='Renier Morales',
      author_email='renier@openhpi.org',
      url='http://www.openhpi.org',
      ext_modules=[Extension('_openhpi',
      			     ['openhpi.i'],
			     libraries=['openhpi'],
			     include_dirs=includes,
			     #extra_compile_args=['-ggdb3'],
			     extra_link_args=['-rdynamic']
			    )
		  ],
      py_modules=['openhpi']
     )

