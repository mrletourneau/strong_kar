# STRONG KAR
VCV Rack plugin implementation of the Karplus Strong algorithm

![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)![Strong Kar](https://github.com/mrletourneau/strong_kar/blob/master/img/glam.png?raw=true)

## About
This module is based on the Karplus Strong algorithm. It works by generating a random wave table upon every pluck, so using two hard-panned instances of this module gives a nice stereo wideness when played simultaneously.

## Building
In the root directory, simply execute `make dist`. See the [VCV Rack Plugin Tutorial](https://vcvrack.com/manual/PluginDevelopmentTutorial.html) for more info.

## Using
`Dampening` controls the decay and dampening speed of the pluck

`Pitch` controls the pitch of the pluck

`Trig` triggers a new pluck

`V/Oct` controls the pitch via CV
