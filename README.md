# Film

A tool for photographers and cinematographers to generate custom film-ish looks for their work.

Want more contrast without completely obliterating highlights?

This is a partial model of color transparency film, with a focus on maintaining color accuracy for
medium contrast settings. This can compensate for loss of saturation caused by increasing fog, and also
for the gain in saturation caused by increasing contrast, by constructing a color profile describing the
results from applying the filter independently to each RGB channel.

This does not inject color casts, or try to look like any particular film emulsion.

# Compiling

`make`

# Usage

See also: [Examples](EXAMPLES.md)

```
Usage: ./cli/film [--title | -t title]
                  [--gamma | -g gamma]
                  [--neutral-color-gamma | -c neutral-color-gamma]
                  [--fog | -f fog]
                  [--print-contrast | -p print contrast]
                  [--colorspace | -s colorspace]
                  [--help | -h]

Options:
  title: the title of the generated LUT
  gamma: the 'gamma', or contrast, of the response curve
  neutral color gamma: the gamma setting at which color saturation is affected
    as little as possible (for medium tones)
  fog: the darkest possible output, in f stops below full output
  print contrast: cascades a second curve so that the output contrast ratio is
    print contrast:1
  colorspace: the output LUT will expect the input image to be in this
    colorspace, and the output image will also be in this colorspace

Available color spaces:
  ciergb
  prophoto
  srgb
  bt709

Program writes the generated LUT as a CUBE file to standard output.
```
