# Understanding Sine Waves

The fundamental mathematics behind Sine waves. Headers come from "A Digital Signal Processing Primer" (Steiglitz)

## 1. Overview

A sine wave is defined by its amplitude, frequency, and phase:

    y(t) = A · sin(2π · f · t + φ)

### 2. Simplest Vibrations
    
    "It is the balance between the two factors - the forcet hat tends to restore the tine to equilibrium,
    and the inertia that tends to make it overshoot  that determines the freqeuncy of oscillation." (pg 2, Steiglitz).

    The initial force equals mass * acceleration, and the restoring force equals a negative proportionality constant (springiness) * position. 

    **F = ma = -kx**

    Larger k gets us a higher frequency, higher m gets us a lower frequency. Think very high note on a guitar, or a large stand up bass. 

    But, "acceleration is just the second derivative of the displacement x with respect to the time variable t" (pg 3, Steiglitz).

    | Quantity | What it measures | Derivative |
    |---|---|---|
    | **Position** `x` | Where you are | — |
    | **Velocity** `v` | How fast position changes | 1st derivative of `x` | -> speed plus direction
    | **Acceleration** `a` | How fast velocity changes | 2nd derivative of `x` | -> change in spead / time taken

    Therefore, we can replace acceleration in F = ma = -kx with a = -(k/m)x and replace a with:

    **d^2x / dt^2** 

    And now, we are looking for a function x(t) that is proportional to this second derivative. 
    More precisely, we are looking for a function whose second derivative is a negative constant times the function itself.
    We want a function that can handle both the push and the pull of **F = ma = -kx**

    d/dt(sin(wt)) = wcos(wt) -> in other words, the first derivative of sin at frequency is cos at frequency (90 degrees apart)

    ***and***

    d/dt(cos(wt)) = -wsin(wt)

    The derivative pulls the w argument out front, that is why we see w on the right-hand side

    And finally!

    **x(t) = sin(w * t + initialAngle)**

### 3. Adding Sinusoids

    Adding two sines of the same freqeuncy, but not necessarily the same phase or amplitude, will create a sine of the same frequency.
    This will be the bedrock of filtering: y[n] = x[n] + a1 · x[n − 1] -> get there a bit later.

    If we were to lock a sinusoid at a phase angle, sin of that angle gives us the y position, and cos of that angle gives us x,
    which together give us the cartesian coordinate of our sinusoid's position at that phase. 

    | Rotation term | Wave term |
    |---|---|
    | One full turn (360° or 2π) | One period |
    | Turns per second | Frequency `f` (Hz) |
    | Angle per second | Angular frequency `ω = 2πf` |
    | Current angle | Phase `θ = ωt + φ` |
    | Circle radius | Amplitude `A` |

### Sines in Shaders

The same function drives smooth motion on the GPU:

    float wave = sin(uv.x * 10.0 + time);

## See Also

- ``Oscillator``
