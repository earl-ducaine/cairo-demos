Fp-Perf-Exp
------------------------------------------------------------------------

This program analyzes performance of fixed point data types against
double.  It uses Ivan Voras' fixedpt library, which provides 64-bit data
types on processors that can support it.

These experiments are intended to inform Cairo Vulkan development.
Cairo uses fixed point internally, but it's unclear if this is strictly
just for core algorithmic requirements or if it has an performance
impact as well.  (Fixed point is sometimes used where the hardware does
not support floating point math, but Cairo uses plenty of floating point
throughout already.)

Fp-Perf-Exp is provided under the gpl-2.0 license.

For more information, see the Cairo mailing list.

### Building

Building Fp-Perf-Exp is fairly straightforward:

    $ git clone <git repository>
    $ cd fp-perf-exp
    $ mkdir build && cd build
    $ meson
    $ ninja
    $ sudo ninja install

### Dependencies

 * None


