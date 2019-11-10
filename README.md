# litest

## Task

In a programming language of your choice (but preferably C++), implement 2-D convolution that uses a
small 8-bit signed vector-matrix multiplier.

## Details

The functionality should be similar to that seen in tf.nn.conv2d, though just the base functionality is sufficient -
not all options need to be considered (e.g. pick single values for padding, data_format, etc).

Assume the input to the Conv2D op is a 8-bit image of width W and height H, C input channels, D output channels,
and batch size of 1. So the entire operation will have two inputs: a 3D input matrix [H, W, C] and a 4D filter
[kh, kw, C, D]. The output is a 3D matrix created by the convolution of the input matrix with the filter. Remember,
all the inputs and outputs should be 8-bit integers. For the multiplier unit, assume you have a function 
similar to (in no particular language):

    z = mult(x, y)

Where:

    z – An 8-bit matrix of shape [N, P], where [zi = xi * y]. i.e. the results of N vector-matrix multiplications
    x – An 8-bit matrix of shape [N, P], essentially N P-sized vectors.
    y – An 8-bit matrix of shape [P, P].

Assume P is fixed, as if we’ve made hardware specifically for that size, but allow input and kernel with any size
width/height to the Conv2D operation.

Pretend we can only do 8-bit signed arithmetic for operations outside of the core multiplier. Annotate the code to point out
where this may be an issue.

Test it.
