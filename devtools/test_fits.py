from astropy.io import fits
h = fits.getheader('/tmp/photyx_test/mono-m82.fit')
for k in ['TESTINT','TESTBOOL','TESTPAD','TESTDATE','EXPTIME']:
    v = h[k]
    print(f'{k}: {v!r} ({type(v).__name__})')
